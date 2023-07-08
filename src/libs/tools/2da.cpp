/*
 * Copyright (c) 2020-2023 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "reone/tools/2da.h"

#include "reone/resource/2da.h"
#include "reone/resource/exception/format.h"
#include "reone/resource/format/2dareader.h"
#include "reone/resource/format/2dawriter.h"
#include "reone/system/binarywriter.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"

#include "tinyxml2.h"

using namespace tinyxml2;

using namespace reone::resource;

namespace reone {

void TwoDaTool::invoke(
    Operation operation,
    const boost::filesystem::path &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

    return invokeBatch(operation, std::vector<boost::filesystem::path> {input}, outputDir, gamePath);
}

void TwoDaTool::invokeBatch(
    Operation operation,
    const std::vector<boost::filesystem::path> &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

    return doInvokeBatch(input, outputDir, [this, &operation](auto &path, auto &outDir) {
        if (operation == Operation::ToXML) {
            toXML(path, outDir);
        } else {
            to2DA(path, outDir);
        }
    });
}

void TwoDaTool::toXML(const boost::filesystem::path &input, const boost::filesystem::path &outputDir) {
    auto twoDa = FileInputStream(input, OpenMode::Binary);

    auto xmlPath = outputDir;
    xmlPath.append(input.filename().string() + ".xml");
    auto xml = FileOutputStream(xmlPath, OpenMode::Binary);

    toXML(twoDa, xml);
}

void TwoDaTool::toXML(IInputStream &twoDa, IOutputStream &xml) {
    auto reader = TwoDaReader(twoDa);
    reader.load();

    auto table = reader.twoDa();
    auto printer = XMLPrinter();
    printer.PushHeader(false, true);
    printer.OpenElement("rows");
    for (int row = 0; row < table->getRowCount(); ++row) {
        printer.OpenElement("row");
        printer.PushAttribute("_index", row);
        for (int col = 0; col < table->getColumnCount(); ++col) {
            printer.PushAttribute(
                table->columns()[col].c_str(),
                table->rows()[row].values[col].c_str());
        }
        printer.CloseElement();
    }
    printer.CloseElement();

    xml.write(printer.CStr(), printer.CStrSize() - 1);
}

void TwoDaTool::to2DA(const boost::filesystem::path &path, const boost::filesystem::path &destPath) {
    auto fp = fopen(path.string().c_str(), "rb");

    auto document = XMLDocument();
    document.LoadFile(fp);

    auto rootElement = document.RootElement();
    if (!rootElement) {
        std::cerr << "XML is empty" << std::endl;
        fclose(fp);
        return;
    }

    auto columns = std::vector<std::string>();
    auto rows = std::vector<TwoDa::Row>();

    // Columns
    auto firstElement = rootElement->FirstChildElement();
    for (auto attribute = firstElement->FirstAttribute(); attribute; attribute = attribute->Next()) {
        if (strncmp(attribute->Name(), "_index", 6) == 0) {
            continue;
        }
        columns.push_back(attribute->Name());
    }

    // Rows
    for (auto element = rootElement->FirstChildElement(); element; element = element->NextSiblingElement()) {
        auto row = TwoDa::Row();
        for (auto attribute = element->FirstAttribute(); attribute; attribute = attribute->Next()) {
            if (strncmp(attribute->Name(), "_index", 6) == 0) {
                continue;
            }
            row.values.push_back(attribute->Value());
        }
        rows.push_back(std::move(row));
    }

    auto twoDa = TwoDa(std::move(columns), std::move(rows));

    std::vector<std::string> tokens;
    boost::split(
        tokens,
        path.filename().string(),
        boost::is_any_of("."),
        boost::token_compress_on);

    auto twoDaPath = boost::filesystem::path(destPath);
    twoDaPath.append(tokens[0] + ".2da");

    auto writer = TwoDaWriter(twoDa);
    writer.save(twoDaPath);
}

bool TwoDaTool::supports(Operation operation, const boost::filesystem::path &input) const {
    return !boost::filesystem::is_directory(input) &&
           ((input.extension() == ".2da" && operation == Operation::ToXML) ||
            (input.extension() == ".xml" && operation == Operation::To2DA));
}

} // namespace reone
