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

#include "reone/tools/lip.h"

#include "reone/graphics/format/lipreader.h"
#include "reone/graphics/format/lipwriter.h"
#include "reone/resource/exception/format.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"

#include "tinyxml2.h"

using namespace tinyxml2;

using namespace reone::graphics;

namespace reone {

void LipTool::invoke(
    Operation operation,
    const boost::filesystem::path &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

    return invokeBatch(operation, std::vector<boost::filesystem::path> {input}, outputDir, gamePath);
}

void LipTool::invokeBatch(
    Operation operation,
    const std::vector<boost::filesystem::path> &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

    return doInvokeBatch(input, outputDir, [this, &operation](auto &path, auto &outDir) {
        if (operation == Operation::ToXML) {
            toXML(path, outDir);
        } else if (operation == Operation::ToLIP) {
            toLIP(path, outDir);
        }
    });
}

void LipTool::toXML(const boost::filesystem::path &path, const boost::filesystem::path &destPath) {
    auto lip = FileInputStream(path, OpenMode::Binary);

    auto xmlPath = destPath;
    xmlPath.append(path.filename().string() + ".xml");
    auto xml = FileOutputStream(xmlPath, OpenMode::Binary);

    toXML(lip, xml);
}

void LipTool::toXML(IInputStream &lip, IOutputStream &xml) {
    auto reader = LipReader(lip, "");
    reader.load();

    auto animation = reader.animation();

    auto printer = XMLPrinter();
    printer.PushHeader(false, true);
    printer.OpenElement("animation");
    printer.PushAttribute("length", animation->length());
    for (auto &keyframe : animation->keyframes()) {
        printer.OpenElement("keyframe");
        printer.PushAttribute("time", keyframe.time);
        printer.PushAttribute("shape", keyframe.shape);
        printer.CloseElement();
    }
    printer.CloseElement();

    xml.write(printer.CStr(), printer.CStrSize() - 1);
}

void LipTool::toLIP(const boost::filesystem::path &path, const boost::filesystem::path &destPath) {
    auto fp = fopen(path.string().c_str(), "rb");

    auto document = XMLDocument();
    document.LoadFile(fp);

    auto rootElement = document.RootElement();
    if (!rootElement) {
        std::cerr << "XML is empty" << std::endl;
        fclose(fp);
        return;
    }

    auto length = rootElement->FloatAttribute("length");
    auto keyframes = std::vector<LipAnimation::Keyframe>();
    for (auto element = rootElement->FirstChildElement(); element; element = element->NextSiblingElement()) {
        keyframes.push_back(LipAnimation::Keyframe {
            element->FloatAttribute("time"),
            static_cast<uint8_t>(element->UnsignedAttribute("shape"))});
    }
    auto animation = LipAnimation("", length, std::move(keyframes));

    std::vector<std::string> tokens;
    boost::split(
        tokens,
        path.filename().string(),
        boost::is_any_of("."),
        boost::token_compress_on);

    auto lipPath = boost::filesystem::path(destPath);
    lipPath.append(tokens[0] + ".lip");

    auto writer = LipWriter(std::move(animation));
    writer.save(lipPath);
}

bool LipTool::supports(Operation operation, const boost::filesystem::path &input) const {
    return !boost::filesystem::is_directory(input) &&
           ((input.extension() == ".lip" && operation == Operation::ToXML) ||
            (input.extension() == ".xml" && operation == Operation::ToLIP));
}

} // namespace reone
