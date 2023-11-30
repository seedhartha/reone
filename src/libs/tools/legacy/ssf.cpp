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

#include "reone/tools/legacy/ssf.h"

#include "reone/resource/exception/format.h"
#include "reone/resource/format/ssfreader.h"
#include "reone/resource/format/ssfwriter.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"

#include "tinyxml2.h"

using namespace tinyxml2;

using namespace reone::resource;

namespace reone {

void SsfTool::invoke(
    Operation operation,
    const std::filesystem::path &input,
    const std::filesystem::path &outputDir,
    const std::filesystem::path &gamePath) {

    return invokeBatch(operation, std::vector<std::filesystem::path> {input}, outputDir, gamePath);
}

void SsfTool::invokeBatch(
    Operation operation,
    const std::vector<std::filesystem::path> &input,
    const std::filesystem::path &outputDir,
    const std::filesystem::path &gamePath) {

    return doInvokeBatch(input, outputDir, [this, &operation](auto &path, auto &outDir) {
        if (operation == Operation::ToXML) {
            toXML(path, outDir);
        } else if (operation == Operation::ToSSF) {
            toSSF(path, outDir);
        }
    });
}

void SsfTool::toXML(const std::filesystem::path &path, const std::filesystem::path &destPath) {
    auto ssf = FileInputStream(path);

    auto xmlPath = destPath;
    xmlPath.append(path.filename().string() + ".xml");
    auto xml = FileOutputStream(xmlPath);

    toXML(ssf, xml);
}

void SsfTool::toXML(IInputStream &ssf, IOutputStream &xml) {
    auto reader = SsfReader(ssf);
    reader.load();

    auto soundSet = reader.soundSet();

    auto printer = XMLPrinter();
    printer.PushHeader(false, true);
    printer.OpenElement("soundset");
    for (size_t i = 0; i < soundSet.size(); ++i) {
        printer.OpenElement("sound");
        printer.PushAttribute("index", static_cast<int>(i));
        printer.PushAttribute("strref", soundSet[i]);
        printer.CloseElement();
    }
    printer.CloseElement();

    xml.write(printer.CStr(), printer.CStrSize() - 1);
}

void SsfTool::toSSF(const std::filesystem::path &path, const std::filesystem::path &destPath) {
    auto fp = fopen(path.string().c_str(), "rb");

    auto document = XMLDocument();
    document.LoadFile(fp);

    auto rootElement = document.RootElement();
    if (!rootElement) {
        std::cerr << "XML is empty" << std::endl;
        fclose(fp);
        return;
    }

    auto soundSet = std::vector<uint32_t>();
    for (auto element = rootElement->FirstChildElement(); element; element = element->NextSiblingElement()) {
        auto strref = element->UnsignedAttribute("strref");
        soundSet.push_back(strref);
    }

    std::vector<std::string> tokens;
    boost::split(
        tokens,
        path.filename().string(),
        boost::is_any_of("."),
        boost::token_compress_on);

    auto ssfPath = std::filesystem::path(destPath);
    ssfPath.append(tokens[0] + ".ssf");

    auto writer = SsfWriter(std::move(soundSet));
    writer.save(ssfPath);
}

bool SsfTool::supports(Operation operation, const std::filesystem::path &input) const {
    return (operation == Operation::ToXML && input.extension() == ".ssf") ||
           (operation == Operation::ToSSF && input.extension() == ".xml");
}

} // namespace reone
