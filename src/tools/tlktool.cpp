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

#include "tools.h"

#include <boost/property_tree/json_parser.hpp>

#include "../engine/resource/format/tlkreader.h"
#include "../engine/resource/format/tlkwriter.h"
#include "../engine/resource/talktable.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void TlkTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToJSON) {
        toJSON(target, destPath);
    } else if (operation == Operation::ToTLK) {
        toTLK(target, destPath);
    }
}

void TlkTool::toJSON(const fs::path &path, const fs::path &destPath) {
    pt::ptree tree;
    pt::ptree children;

    TlkReader tlk;
    tlk.load(path);

    shared_ptr<TalkTable> table(tlk.table());
    for (int i = 0; i < table->getStringCount(); ++i) {
        const TalkTableString &tableString = table->getString(i);

        pt::ptree child;
        child.put("_index", i);
        child.put("text", tableString.text);
        child.put("soundResRef", tableString.soundResRef);

        children.push_back(make_pair("", child));
    }
    tree.add_child("strings", children);

    fs::path jsonPath(destPath);
    jsonPath.append(path.filename().string() + ".json");

    pt::write_json(jsonPath.string(), tree);
}

void TlkTool::toTLK(const fs::path &path, const fs::path &destPath) {
    pt::ptree tree;
    pt::read_json(path.string(), tree);

    auto talkTable = make_shared<TalkTable>();
    for (auto &str : tree.get_child("strings")) {
        TalkTableString talkTableString;
        talkTableString.text = str.second.get("text", "");
        talkTableString.soundResRef = str.second.get("soundResRef", "");
        talkTable->addString(move(talkTableString));
    }

    fs::path extensionless(path);
    extensionless.replace_extension(); // remove .json
    if (extensionless.extension() == ".tlk") {
        extensionless.replace_extension();
    }

    fs::path tlkPath(destPath);
    tlkPath.append(extensionless.filename().string() + ".tlk");

    TlkWriter writer(move(talkTable));
    writer.save(tlkPath);
}

bool TlkTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           ((target.extension() == ".tlk" && operation == Operation::ToJSON) || (target.extension() == ".json" && operation == Operation::ToTLK));
}

} // namespace tools

} // namespace reone
