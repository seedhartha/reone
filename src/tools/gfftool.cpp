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

#include "../engine/common/streamwriter.h"
#include "../engine/resource/format/gffreader.h"
#include "../engine/resource/format/gffwriter.h"
#include "../engine/resource/typeutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void GffTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    switch (operation) {
        case Operation::ToJSON:
            toJSON(target, destPath);
            break;
        case Operation::ToGFF:
            toGFF(target, destPath);
            break;
        default:
            break;
    }
}

static pt::ptree getPropertyTree(const GffStruct &gffs) {
    pt::ptree fields;
    for (auto &field : gffs.fields()) {
        fields.put(field.label, static_cast<int>(field.type));
    }

    pt::ptree tree;
    tree.put("_type", gffs.type());
    tree.add_child("_fields", fields);

    for (auto &field : gffs.fields()) {
        switch (field.type) {
            case GffStruct::FieldType::Byte:
            case GffStruct::FieldType::Word:
            case GffStruct::FieldType::Dword:
                tree.put(field.label, field.uintValue);
                break;
            case GffStruct::FieldType::Char:
            case GffStruct::FieldType::Short:
            case GffStruct::FieldType::Int:
            case GffStruct::FieldType::StrRef:
                tree.put(field.label, field.intValue);
                break;
            case GffStruct::FieldType::Dword64:
                tree.put(field.label, field.uint64Value);
                break;
            case GffStruct::FieldType::Int64:
                tree.put(field.label, field.int64Value);
                break;
            case GffStruct::FieldType::Float:
                tree.put(field.label, field.floatValue);
                break;
            case GffStruct::FieldType::Double:
                tree.put(field.label, field.doubleValue);
                break;
            case GffStruct::FieldType::CExoString:
            case GffStruct::FieldType::ResRef:
                tree.put(field.label, field.strValue);
                break;
            case GffStruct::FieldType::CExoLocString:
                tree.put(field.label, boost::format("%d|%s") % field.intValue % field.strValue);
                break;
            case GffStruct::FieldType::Void: {
                string value;
                value.resize(2 * field.data.size());
                for (size_t i = 0; i < field.data.size(); ++i) {
                    sprintf(&value[2 * i], "%02hhx", field.data[i]);
                }
                tree.put(field.label, value);
                break;
            }
            case GffStruct::FieldType::Struct: {
                tree.add_child(field.label, getPropertyTree(*field.children[0]));
                break;
            }
            case GffStruct::FieldType::List: {
                pt::ptree children;
                for (auto &child : field.children) {
                    children.push_back(make_pair("", getPropertyTree(*child)));
                }
                tree.add_child(field.label, children);
                break;
            }
            case GffStruct::FieldType::Orientation:
                tree.put(field.label, boost::format("%f|%f|%f|%f") % field.quatValue.w % field.quatValue.x % field.quatValue.y % field.quatValue.z);
                break;
            case GffStruct::FieldType::Vector:
                tree.put(field.label, boost::format("%f|%f|%f") % field.vecValue.x % field.vecValue.y % field.vecValue.z);
                break;
            default:
                cerr << "Unsupported GFF field type: " << to_string(static_cast<int>(field.type)) << endl;
                break;
        }
    }

    return move(tree);
}

void GffTool::toJSON(const fs::path &path, const fs::path &destPath) {
    GffReader gff;
    gff.load(path);

    shared_ptr<GffStruct> gffs(gff.root());
    pt::ptree tree(getPropertyTree(*gffs));

    fs::path jsonPath(destPath);
    jsonPath.append(path.filename().string() + ".json");

    pt::write_json(jsonPath.string(), tree);
}

static unique_ptr<GffStruct> treeToGffStruct(const pt::ptree &tree) {
    map<string, GffStruct::FieldType> fieldTypes;
    if (tree.count("_fields") > 0) {
        for (auto &child : tree.get_child("_fields")) {
            fieldTypes.insert(make_pair(child.first, static_cast<GffStruct::FieldType>(child.second.get_value<int>())));
        }
    }

    auto gffs = make_unique<GffStruct>(tree.get<uint32_t>("_type"));

    vector<string> tokens;
    for (auto &child : tree) {
        // Properties with a leading underscore are metadata
        if (boost::starts_with(child.first, "_")) continue;

        GffStruct::Field field;
        field.type = fieldTypes[child.first];
        field.label = child.first;

        switch (field.type) {
            case GffStruct::FieldType::Byte:
            case GffStruct::FieldType::Word:
            case GffStruct::FieldType::Dword:
                field.uintValue = child.second.get_value<uint32_t>();
                break;
            case GffStruct::FieldType::Char:
            case GffStruct::FieldType::Short:
            case GffStruct::FieldType::Int:
                field.intValue = child.second.get_value<int32_t>();
                break;
            case GffStruct::FieldType::Dword64:
                field.uint64Value = child.second.get_value<uint64_t>();
                break;
            case GffStruct::FieldType::Int64:
                field.int64Value = child.second.get_value<int64_t>();
                break;
            case GffStruct::FieldType::Float:
                field.floatValue = child.second.get_value<float>();
                break;
            case GffStruct::FieldType::Double:
                field.doubleValue = child.second.get_value<double>();
                break;
            case GffStruct::FieldType::CExoString:
                field.strValue = child.second.get_value<string>();
                break;
            case GffStruct::FieldType::ResRef:
                field.strValue = child.second.get_value<string>();
                break;
            case GffStruct::FieldType::CExoLocString: {
                string value(child.second.get_value<string>());
                boost::split(tokens, value, boost::is_any_of("|"), boost::token_compress_on);
                field.intValue = stoi(tokens[0]);
                field.strValue = tokens[1];
                break;
            }
            case GffStruct::FieldType::Void: {
                string value(child.second.get_value<string>());
                for (size_t i = 0; i < value.length(); i += 2) {
                    uint8_t byte;
                    if (sscanf(value.c_str(), "%02hhx", &byte)) {
                        field.data.push_back(*reinterpret_cast<char *>(&byte));
                    }
                }
                break;
            }
            case GffStruct::FieldType::Struct:
                field.children.push_back(treeToGffStruct(child.second));
                break;
            case GffStruct::FieldType::List:
                if (!child.second.empty()) {
                    for (auto &childChild : child.second) {
                        field.children.push_back(treeToGffStruct(childChild.second));
                    }
                }
                break;
            case GffStruct::FieldType::Orientation: {
                string value(child.second.get_value<string>());
                boost::split(tokens, value, boost::is_any_of("|"), boost::token_compress_on);
                field.quatValue = glm::quat(stof(tokens[0]), stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                break;
            }
            case GffStruct::FieldType::Vector: {
                string value(child.second.get_value<string>());
                boost::split(tokens, value, boost::is_any_of("|"), boost::token_compress_on);
                field.vecValue = glm::vec3(stof(tokens[0]), stof(tokens[1]), stof(tokens[2]));
                break;
            }
            case GffStruct::FieldType::StrRef:
                field.intValue = child.second.get_value<int32_t>();
                break;
            default:
                throw runtime_error("Unsupported field type: " + to_string(static_cast<int>(field.type)));
        }

        gffs->add(move(field));
    }

    return move(gffs);
}

void GffTool::toGFF(const fs::path &path, const fs::path &destPath) {
    if (path.extension() != ".json") {
        cerr << "Input file must have extension '.json'" << endl;
        return;
    }

    pt::ptree tree;
    pt::read_json(path.string(), tree);

    fs::path extensionless(path);
    extensionless.replace_extension();
    ResourceType resType = getResTypeByExt(extensionless.extension().string().substr(1));

    fs::path gffPath(destPath);
    gffPath.append(extensionless.filename().string());

    GffWriter writer(resType, treeToGffStruct(tree));
    writer.save(gffPath);
}

bool GffTool::supports(Operation operation, const fs::path &target) const {
    return
        !fs::is_directory(target) &&
        (operation == Operation::ToJSON || operation == Operation::ToGFF);
}

} // namespace tools

} // namespace reone
