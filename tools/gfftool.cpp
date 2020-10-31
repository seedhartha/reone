/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <iomanip>
#include <iostream>

#include <boost/property_tree/json_parser.hpp>

#include "../src/resource/gfffile.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void GffTool::convert(const fs::path &path, const fs::path &destPath) const {
    GffFile gff;
    gff.load(path);

    shared_ptr<GffStruct> gffs(gff.top());
    pt::ptree tree(getPropertyTree(*gffs));

    fs::path jsonPath(destPath);
    jsonPath.append(path.filename().string() + ".json");

    fs::ofstream json(jsonPath);
    pt::write_json(json, tree);
}

pt::ptree GffTool::getPropertyTree(const GffStruct &gffs) const {
    pt::ptree tree;
    pt::ptree child;
    pt::ptree children;
    vector<float> values;

    for (auto &field : gffs.fields()) {
        auto &fieldChildren = field.children();

        switch (field.type()) {
            case GffFieldType::Struct:
                child = getPropertyTree(field.children()[0]);
                tree.add_child(field.label(), child);
                break;

            case GffFieldType::List:
                children.clear();
                for (auto &childGffs : fieldChildren) {
                    child = getPropertyTree(childGffs);
                    children.push_back(make_pair("", child));
                }
                tree.add_child(field.label(), children);
                break;

            case GffFieldType::Orientation:
                values = field.asFloatArray();
                child.clear();
                child.put("A", values[0]);
                child.put("B", values[1]);
                child.put("C", values[2]);
                child.put("D", values[3]);
                tree.add_child(field.label(), child);
                break;

            case GffFieldType::Vector:
                values = field.asFloatArray();
                child.clear();
                child.put("X", values[0]);
                child.put("Y", values[1]);
                child.put("Z", values[2]);
                tree.add_child(field.label(), child);
                break;

            default:
                tree.put(field.label(), field.asString());
                break;
        }
    }

    return tree;
}

} // namespace tools

} // namespace reone
