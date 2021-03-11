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

#include "../src/resource/format/tlkreader.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void TlkTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToJSON) {
        toJSON(target, destPath);
    }
}

void TlkTool::toJSON(const fs::path &path, const fs::path &destPath) {
    pt::ptree tree;
    pt::ptree children;

    TlkReader tlk;
    tlk.load(path);

    shared_ptr<TalkTable> table(tlk.table());
    for (int i = 0; i < table->getStringCount(); ++i) {
        string s(table->getString(i).text);

        pt::ptree child;
        child.put("index", i);
        child.put("string", s);

        children.push_back(make_pair("", child));
    }
    tree.add_child("strings", children);

    fs::path jsonPath(destPath);
    jsonPath.append(path.filename().string() + ".json");

    pt::write_json(jsonPath.string(), tree);
}

bool TlkTool::supports(Operation operation, const fs::path &target) const {
    return
        !fs::is_directory(target) &&
        target.extension() == ".tlk" &&
        operation == Operation::ToJSON;
}

} // namespace tools

} // namespace reone
