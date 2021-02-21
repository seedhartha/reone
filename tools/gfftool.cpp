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
    pt::ptree child;
    pt::ptree children;
    vector<float> values;

    for (auto &field : gffs.fields()) {
        auto &fieldChildren = field->children;

        switch (field->type) {
            case GffStruct::FieldType::Byte:
            case GffStruct::FieldType::Word:
            case GffStruct::FieldType::Dword:
                tree.put(field->label, field->uintValue);
                break;
            case GffStruct::FieldType::Char:
            case GffStruct::FieldType::Short:
            case GffStruct::FieldType::Int:
            case GffStruct::FieldType::StrRef:
                tree.put(field->label, field->intValue);
                break;
            case GffStruct::FieldType::Dword64:
                tree.put(field->label, field->uint64Value);
                break;
            case GffStruct::FieldType::Int64:
                tree.put(field->label, field->int64Value);
                break;
            case GffStruct::FieldType::Float:
                tree.put(field->label, field->floatValue);
                break;
            case GffStruct::FieldType::Double:
                tree.put(field->label, field->doubleValue);
                break;
            case GffStruct::FieldType::CExoString:
            case GffStruct::FieldType::ResRef:
                tree.put(field->label, field->strValue);
                break;
            case GffStruct::FieldType::CExoLocString:
                child.clear();
                child.put("StrRef", field->intValue);
                child.put("Substring", field->strValue);
                tree.add_child(field->label, child);
                break;
            case GffStruct::FieldType::Struct:
                child = getPropertyTree(*field->children[0]);
                tree.add_child(field->label, child);
                break;
            case GffStruct::FieldType::List:
                children.clear();
                for (auto &childGffs : fieldChildren) {
                    child = getPropertyTree(*childGffs);
                    children.push_back(make_pair("", child));
                }
                tree.add_child(field->label, children);
                break;
            case GffStruct::FieldType::Orientation:
                child.clear();
                child.put("W", field->quatValue[0]);
                child.put("X", field->quatValue[1]);
                child.put("Y", field->quatValue[2]);
                child.put("Z", field->quatValue[3]);
                tree.add_child(field->label, child);
                break;
            case GffStruct::FieldType::Vector:
                child.clear();
                child.put("X", field->vecValue[0]);
                child.put("Y", field->vecValue[1]);
                child.put("Z", field->vecValue[2]);
                tree.add_child(field->label, child);
                break;
            default:
                cerr << "Unsupported GFF field type: " << to_string(static_cast<int>(field->type)) << endl;
                break;
        }
    }

    return tree;
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
