/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "tlk.h"

#include "../../common/stream/fileinput.h"
#include "../../resource/format/tlkreader.h"
#include "../../resource/format/tlkwriter.h"
#include "../../resource/talktable.h"

#include "tinyxml2.h"

using namespace std;

using namespace tinyxml2;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

void TlkTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToXML) {
        toXML(target, destPath);
    } else if (operation == Operation::ToTLK) {
        toTLK(target, destPath);
    }
}

void TlkTool::toXML(const fs::path &path, const fs::path &destPath) {
    auto stream = FileInputStream(path, OpenMode::Binary);

    auto reader = TlkReader();
    reader.load(stream);

    auto table = reader.table();

    auto xmlPath = destPath;
    xmlPath.append(path.filename().string() + ".xml");
    auto fp = fopen(xmlPath.string().c_str(), "wb");

    auto printer = XMLPrinter(fp);
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

    fclose(fp);
}

void TlkTool::toTLK(const fs::path &path, const fs::path &destPath) {
    auto fp = fopen(path.string().c_str(), "rb");

    auto document = XMLDocument();
    document.LoadFile(fp);

    auto rootElement = document.RootElement();
    if (!rootElement) {
        cerr << "XML is empty" << endl;
        fclose(fp);
        return;
    }

    auto strings = vector<TalkTable::String>();
    for (auto element = rootElement->FirstChildElement(); element; element = element->NextSiblingElement()) {
        strings.push_back(TalkTable::String {
            element->Attribute("text"),
            element->Attribute("soundResRef")});
    }
    auto table = TalkTable(move(strings));

    vector<string> tokens;
    boost::split(
        tokens,
        path.filename().string(),
        boost::is_any_of("."),
        boost::token_compress_on);

    auto tlkPath = fs::path(destPath);
    tlkPath.append(tokens[0] + ".tlk");

    auto writer = TlkWriter(table);
    writer.save(tlkPath);
}

bool TlkTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           ((target.extension() == ".tlk" && operation == Operation::ToXML) ||
            (target.extension() == ".xml" && operation == Operation::ToTLK));
}

} // namespace reone
