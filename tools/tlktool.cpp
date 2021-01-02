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

#include "../src/resource/tlkfile.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void TlkTool::convert(const fs::path &path, const fs::path &destPath) const {
    pt::ptree tree;
    pt::ptree children;

    TlkFile tlk;
    tlk.load(path);

    shared_ptr<TalkTable> table(tlk.table());
    for (int i = 0; i < table->stringCount(); ++i) {
        string s(table->getString(i).text);

        pt::ptree child;
        child.put("index", i);
        child.put("string", s);

        children.push_back(make_pair("", child));
    }
    tree.add_child("strings", children);

    fs::path jsonPath(destPath);
    jsonPath.append(path.filename().string() + ".json");

    fs::ofstream json(jsonPath);
    pt::write_json(json, tree);
}

} // namespace tools

} // namespace reone
