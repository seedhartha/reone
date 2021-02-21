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

#include <iostream>

#include <boost/property_tree/json_parser.hpp>

#include "../src/resource/format/gfffile.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void GffTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToJSON) {
        toJSON(target, destPath);
    }
}

static pt::ptree getPropertyTree(const GffStruct &gffs) {
    pt::ptree tree;

    for (auto &field : gffs.fields()) {
        string id(str(boost::format("%s|%d") % field->label % static_cast<int>(field->type)));

        switch (field->type) {
            case GffStruct::FieldType::Byte:
            case GffStruct::FieldType::Word:
            case GffStruct::FieldType::Dword:
                tree.put(id, field->uintValue);
                break;
            case GffStruct::FieldType::Char:
            case GffStruct::FieldType::Short:
            case GffStruct::FieldType::Int:
            case GffStruct::FieldType::StrRef:
                tree.put(id, field->intValue);
                break;
            case GffStruct::FieldType::Dword64:
                tree.put(id, field->uint64Value);
                break;
            case GffStruct::FieldType::Int64:
                tree.put(id, field->int64Value);
                break;
            case GffStruct::FieldType::Float:
                tree.put(id, field->floatValue);
                break;
            case GffStruct::FieldType::Double:
                tree.put(id, field->doubleValue);
                break;
            case GffStruct::FieldType::CExoString:
            case GffStruct::FieldType::ResRef:
                tree.put(id, field->strValue);
                break;
            case GffStruct::FieldType::CExoLocString:
                tree.put(id, boost::format("%d|%s") % field->intValue % field->strValue);
                break;
            case GffStruct::FieldType::Struct: {
                tree.add_child(id, getPropertyTree(*field->children[0]));
                break;
            }
            case GffStruct::FieldType::List: {
                pt::ptree children;
                for (auto &child : field->children) {
                    children.push_back(make_pair("", getPropertyTree(*child)));
                }
                tree.add_child(id, children);
                break;
            }
            case GffStruct::FieldType::Orientation:
                tree.put(id, boost::format("%f|%f|%f|%f") % field->quatValue[0] % field->quatValue[1] % field->quatValue[2] % field->quatValue[3]);
                break;
            case GffStruct::FieldType::Vector:
                tree.put(id, boost::format("%f|%f|%f") % field->vecValue[0] % field->vecValue[1] % field->vecValue[2]);
                break;
            default:
                cerr << "Unsupported GFF field type: " << to_string(static_cast<int>(field->type)) << endl;
                break;
        }
    }

    return move(tree);
}

void GffTool::toJSON(const fs::path &path, const fs::path &destPath) {
    GffFile gff;
    gff.load(path);

    shared_ptr<GffStruct> gffs(gff.root());
    pt::ptree tree(getPropertyTree(*gffs));

    fs::path jsonPath(destPath);
    jsonPath.append(path.filename().string() + ".json");

    pt::write_json(jsonPath.string(), tree);
}

bool GffTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) && operation == Operation::ToJSON;
}

} // namespace tools

} // namespace reone
