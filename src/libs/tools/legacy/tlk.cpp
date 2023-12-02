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

#include "reone/tools/legacy/tlk.h"

#include "reone/resource/format/tlkreader.h"
#include "reone/resource/format/tlkwriter.h"
#include "reone/resource/talktable.h"
#include "reone/system/exception/validation.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"

#include "tinyxml2.h"

using namespace tinyxml2;

using namespace reone::resource;

namespace reone {

void TlkTool::invoke(
    Operation operation,
    const std::filesystem::path &input,
    const std::filesystem::path &outputDir,
    const std::filesystem::path &gamePath) {

    return invokeBatch(operation, std::vector<std::filesystem::path> {input}, outputDir, gamePath);
}

void TlkTool::invokeBatch(
    Operation operation,
    const std::vector<std::filesystem::path> &input,
    const std::filesystem::path &outputDir,
    const std::filesystem::path &gamePath) {

    return doInvokeBatch(input, outputDir, [this, &operation](auto &path, auto &outDir) {
        if (operation == Operation::ToXML) {
            toXML(path, outDir);
        } else if (operation == Operation::ToTLK) {
            toTLK(path, outDir);
        }
    });
}

void TlkTool::toXML(const std::filesystem::path &path, const std::filesystem::path &destPath) {
    auto tlk = FileInputStream(path);

    auto xmlPath = destPath;
    xmlPath.append(path.filename().string() + ".xml");
    auto xml = FileOutputStream(xmlPath);

    toXML(tlk, xml);
}

void TlkTool::toXML(IInputStream &tlk, IOutputStream &xml) {
    auto reader = TlkReader(tlk);
    reader.load();

    auto table = reader.table();
    auto printer = XMLPrinter();
    printer.PushHeader(false, true);
    printer.OpenElement("strings");
    for (int i = 0; i < table->getStringCount(); ++i) {
        printer.OpenElement("string");
        printer.PushAttribute("index", i);
        printer.PushAttribute("text", table->getString(i).text.c_str());
        printer.PushAttribute("soundResRef", table->getString(i).soundResRef.c_str());
        printer.CloseElement();
    }
    printer.CloseElement();

    xml.write(printer.CStr(), printer.CStrSize() - 1);
}

void TlkTool::toTLK(const std::filesystem::path &path, const std::filesystem::path &destPath) {
    auto fp = fopen(path.string().c_str(), "rb");

    auto document = XMLDocument();
    document.LoadFile(fp);

    auto rootElement = document.RootElement();
    if (!rootElement) {
        std::cerr << "XML is empty" << std::endl;
        fclose(fp);
        return;
    }

    auto strings = std::vector<TalkTable::String>();
    for (auto element = rootElement->FirstChildElement(); element; element = element->NextSiblingElement()) {
        strings.push_back(TalkTable::String {
            element->Attribute("text"),
            element->Attribute("soundResRef")});
    }
    auto table = TalkTable(std::move(strings));

    std::vector<std::string> tokens;
    boost::split(
        tokens,
        path.filename().string(),
        boost::is_any_of("."),
        boost::token_compress_on);

    auto tlkPath = std::filesystem::path(destPath);
    tlkPath.append(tokens[0] + ".tlk");

    auto writer = TlkWriter(table);
    writer.save(tlkPath);
}

bool TlkTool::supports(Operation operation, const std::filesystem::path &input) const {
    return !std::filesystem::is_directory(input) &&
           ((input.extension() == ".tlk" && operation == Operation::ToXML) ||
            (input.extension() == ".xml" && operation == Operation::ToTLK));
}

} // namespace reone
