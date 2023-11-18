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

#include "gffschema.h"

#include "reone/resource/format/gffreader.h"
#include "reone/resource/gff.h"
#include "reone/resource/provider/erf.h"
#include "reone/resource/provider/keybif.h"
#include "reone/resource/provider/rim.h"
#include "reone/resource/typeutil.h"
#include "reone/system/fileutil.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/textwriter.h"

#include "templates.h"

using namespace reone::resource;

namespace reone {

struct SchemaStruct;

struct SchemaField {
    Gff::FieldType type {Gff::FieldType::Void};
    std::string name;
    std::string cppName;
    bool optional {false};
    std::unique_ptr<SchemaStruct> subStruct;
};

struct SchemaStruct {
    std::string name;
    std::map<std::string, SchemaField> fields;
    bool top {false};
};

static void appendGffToSchema(Gff &tree, SchemaStruct &schemaStruct) {
    std::set<std::string> fields;
    for (auto &field : tree.fields()) {
        if (schemaStruct.fields.count(field.label) == 0) {
            SchemaField sf;
            sf.type = field.type;
            sf.name = field.label;
            sf.cppName = boost::replace_all_copy(field.label, " ", "_");
            schemaStruct.fields[field.label] = std::move(sf);
        }
        auto &schemaField = schemaStruct.fields.at(field.label);
        if ((field.type == Gff::FieldType::Struct || field.type == Gff::FieldType::List) && !field.children.empty()) {
            if (!schemaField.subStruct) {
                schemaField.subStruct = std::make_unique<SchemaStruct>();
                schemaField.subStruct->name = str(boost::format("%s_%s") % schemaStruct.name % schemaField.cppName);
            }
            for (auto &child : field.children) {
                appendGffToSchema(*field.children[0], *schemaField.subStruct);
            }
        }
        fields.insert(field.label);
    }
    for (auto &[name, schemaField] : schemaStruct.fields) {
        if (fields.count(name) == 0) {
            schemaField.optional = true;
        }
    }
}

static std::string describeFieldType(const SchemaField &field) {
    switch (field.type) {
    case Gff::FieldType::Byte:
        return "uint8_t";
    case Gff::FieldType::Char:
        return "char";
    case Gff::FieldType::Word:
        return "uint16_t";
    case Gff::FieldType::Short:
        return "int16_t";
    case Gff::FieldType::Dword:
        return "uint32_t";
    case Gff::FieldType::Int:
        return "int";
    case Gff::FieldType::Dword64:
        return "uint64_t";
    case Gff::FieldType::Int64:
        return "int64_t";
    case Gff::FieldType::Float:
        return "float";
    case Gff::FieldType::Double:
        return "double";
    case Gff::FieldType::CExoString:
        return "std::string";
    case Gff::FieldType::ResRef:
        return "std::string";
    case Gff::FieldType::CExoLocString:
        return "std::pair<int, std::string>";
    case Gff::FieldType::Void:
        return "std::vector<char>";
    case Gff::FieldType::Struct:
        if (field.subStruct) {
            return field.subStruct->name;
        } else {
            return "void *";
        }
    case Gff::FieldType::List:
        if (field.subStruct) {
            return str(boost::format("std::vector<%s>") % field.subStruct->name);
        } else {
            return "std::vector<void *>";
        }
    case Gff::FieldType::Orientation:
        return "glm::quat";
    case Gff::FieldType::Vector:
        return "glm::vec3";
    case Gff::FieldType::StrRef:
        return "int";
    default:
        throw std::invalid_argument("Invalid GFF field type: " + std::to_string(static_cast<int>(field.type)));
    }
}

static void writeField(const SchemaField &field, TextWriter &writer) {
    auto typeStr = describeFieldType(field);
    switch (field.type) {
    case Gff::FieldType::Byte:
    case Gff::FieldType::Word:
    case Gff::FieldType::Short:
    case Gff::FieldType::Dword:
    case Gff::FieldType::Int:
    case Gff::FieldType::Dword64:
    case Gff::FieldType::Int64:
    case Gff::FieldType::StrRef:
        writer.write(str(boost::format("%s%s %s {0};\n") % kIndent % typeStr % field.cppName));
        break;
    case Gff::FieldType::Float:
        writer.write(str(boost::format("%s%s %s {0.0f};\n") % kIndent % typeStr % field.cppName));
        break;
    case Gff::FieldType::Double:
        writer.write(str(boost::format("%s%s %s {0.0};\n") % kIndent % typeStr % field.cppName));
        break;
    case Gff::FieldType::Char:
        writer.write(str(boost::format("%s%s %s {'\\0'};\n") % kIndent % typeStr % field.cppName));
        break;
    case Gff::FieldType::Orientation:
        writer.write(str(boost::format("%s%s %s {1.0f, 0.0f, 0.0f, 0.0f};\n") % kIndent % typeStr % field.cppName));
        break;
    case Gff::FieldType::Vector:
        writer.write(str(boost::format("%s%s %s {0.0f};\n") % kIndent % typeStr % field.cppName));
        break;
    default:
        writer.write(str(boost::format("%s%s %s;\n") % kIndent % typeStr % field.cppName));
        break;
    }
}

static void writeStruct(const SchemaStruct &schemaStruct, TextWriter &writer) {
    writer.write(str(boost::format("struct %s {\n") % schemaStruct.name));
    for (auto &[_, field] : schemaStruct.fields) {
        writeField(field, writer);
    }
    writer.write("};\n\n");
}

static void writeSchemaHeaderFile(const std::string &topStructName,
                                  const std::vector<std::pair<int, SchemaStruct *>> &structs,
                                  const std::filesystem::path &path) {
    auto stream = FileOutputStream(path);
    auto writer = TextWriter(stream);
    writer.write(kCopyrightNotice);
    writer.write("\n\n");
    writer.write("#pragma once\n\n");
    writer.write("namespace reone {\n\n");
    writer.write("namespace resource {\n\n");
    writer.write("class Gff;\n\n");
    writer.write("}\n\n");
    writer.write("namespace game {\n\n");
    writer.write("namespace gffschema {\n\n");
    for (auto &[_, schemaStruct] : structs) {
        writeStruct(*schemaStruct, writer);
    }
    for (auto &[_, schemaStruct] : structs) {
        if (schemaStruct->top) {
            writer.write(str(boost::format("%1% parse%1%(const resource::Gff &gff);\n") % topStructName));
        }
    }
    writer.write("\n");
    writer.write("} // namespace gffschema\n\n");
    writer.write("} // namespace game\n\n");
    writer.write("} // namespace reone\n");
}

static void writeParseFunction(const SchemaStruct &schemaStruct, TextWriter &writer) {
    if (schemaStruct.top) {
        writer.write(str(boost::format("%1% parse%1%(const Gff &gff) {\n") % schemaStruct.name));
    } else {
        writer.write(str(boost::format("static %1% parse%1%(const Gff &gff) {\n") % schemaStruct.name));
    }
    writer.write(str(boost::format("%s%s strct;\n") % kIndent % schemaStruct.name));
    for (auto &[_, field] : schemaStruct.fields) {
        switch (field.type) {
        case Gff::FieldType::Byte:
        case Gff::FieldType::Word:
        case Gff::FieldType::Dword:
            writer.write(str(boost::format("%1%strct.%2% = gff.getUint(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        case Gff::FieldType::Char:
        case Gff::FieldType::Short:
        case Gff::FieldType::Int:
        case Gff::FieldType::StrRef:
            writer.write(str(boost::format("%1%strct.%2% = gff.getInt(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        case Gff::FieldType::Dword64:
            writer.write(str(boost::format("%1%strct.%2% = gff.readUint64(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        case Gff::FieldType::Int64: {
            writer.write(str(boost::format("%1%strct.%2% = gff.readInt64(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        }
        case Gff::FieldType::Float:
            writer.write(str(boost::format("%1%strct.%2% = gff.getFloat(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        case Gff::FieldType::Double:
            writer.write(str(boost::format("%1%strct.%2% = gff.getDouble(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        case Gff::FieldType::CExoString:
        case Gff::FieldType::ResRef:
            writer.write(str(boost::format("%1%strct.%2% = gff.getString(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        case Gff::FieldType::CExoLocString:
            writer.write(str(boost::format("%1%strct.%2% = std::make_pair(gff.getInt(\"%3%\"), gff.getString(\"%3%\"));\n") % kIndent % field.cppName % field.name));
            break;
        case Gff::FieldType::Void:
            writer.write(str(boost::format("%1%strct.%2% = gff.getData(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        case Gff::FieldType::Struct:
            writer.write(str(boost::format("%1%auto %2% = gff.findStruct(\"%3%\");\n") % kIndent % field.cppName % field.name));
            writer.write(str(boost::format("%1%if (%2%) {\n") % kIndent % field.cppName));
            writer.write(str(boost::format("%1%%1%strct.%2% = parse%3%(*%2%);\n") % kIndent % field.cppName % field.subStruct->name));
            writer.write(str(boost::format("%1%}\n") % kIndent));
            break;
        case Gff::FieldType::List:
            if (field.subStruct) {
                writer.write(str(boost::format("%1%for (auto &item : gff.getList(\"%2%\")) {\n") % kIndent % field.name));
                writer.write(str(boost::format("%1%%1%strct.%2%.push_back(parse%3%(*item));\n") % kIndent % field.cppName % field.subStruct->name));
                writer.write(str(boost::format("%1%}\n") % kIndent));
            }
            break;
        case Gff::FieldType::Orientation:
            writer.write(str(boost::format("%1%strct.%2% = gff.getOrientation(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        case Gff::FieldType::Vector:
            writer.write(str(boost::format("%1%strct.%2% = gff.getVector(\"%3%\");\n") % kIndent % field.cppName % field.name));
            break;
        default:
            throw std::logic_error("Invalid field type: " + std::to_string(static_cast<int>(field.type)));
        }
    }
    writer.write(str(boost::format("%sreturn strct;\n") % kIndent));
    writer.write("}\n\n");
}

static void writeSchemaImplFile(const std::vector<std::pair<int, SchemaStruct *>> &structs,
                                const std::string &schemaHeaderFilename,
                                const std::filesystem::path &path) {
    auto stream = FileOutputStream(path);
    auto writer = TextWriter(stream);
    writer.write(kCopyrightNotice);
    writer.write("\n\n");
    writer.write(str(boost::format(kIncludeFormat + "\n\n") % schemaHeaderFilename));
    writer.write(str(boost::format(kIncludeFormat + "\n\n") % "reone/resource/gff.h"));
    writer.write("using namespace reone::resource;\n\n");
    writer.write("namespace reone {\n\n");
    writer.write("namespace game {\n\n");
    writer.write("namespace gffschema {\n\n");
    for (auto &[_, schemaStruct] : structs) {
        writeParseFunction(*schemaStruct, writer);
    }
    writer.write("} // namespace gffschema\n\n");
    writer.write("} // namespace game\n\n");
    writer.write("} // namespace reone\n");
}

void generateGffSchema(resource::ResourceType resType,
                       const std::filesystem::path &k1dir,
                       const std::filesystem::path &k2dir,
                       const std::filesystem::path &destDir) {
    std::map<std::string, std::shared_ptr<Gff>> trees;

    auto keyPath = getFileIgnoreCase(k2dir, "chitin.key");
    auto keyBif = KeyBifResourceProvider(keyPath);
    keyBif.init();
    for (auto &resId : keyBif.resourceIds()) {
        if (resId.type != resType) {
            continue;
        }
        auto bytes = keyBif.findResourceData(resId);
        auto stream = MemoryInputStream(*bytes);
        auto reader = GffReader(stream);
        reader.load();
        trees[resId.resRef] = reader.root();
    }

    auto modulesPath = getFileIgnoreCase(k2dir, "modules");
    for (auto &entry : std::filesystem::directory_iterator(modulesPath)) {
        if (!std::filesystem::is_regular_file(entry)) {
            continue;
        }
        auto extension = boost::to_lower_copy(entry.path().extension().string());
        if (extension == ".rim") {
            auto rim = RimResourceProvider(entry.path());
            rim.init();
            for (auto &res : rim.resourceIds()) {
                if (res.type != resType) {
                    continue;
                }
                auto bytes = rim.findResourceData(res);
                auto stream = MemoryInputStream(*bytes);
                auto reader = GffReader(stream);
                reader.load();
                trees[res.resRef] = reader.root();
            }
        } else if (extension == ".erf") {
            auto erf = ErfResourceProvider(entry.path());
            erf.init();
            for (auto &res : erf.resourceIds()) {
                if (res.type != resType) {
                    continue;
                }
                auto bytes = erf.findResourceData(res);
                auto stream = MemoryInputStream(*bytes);
                auto reader = GffReader(stream);
                reader.load();
                trees[res.resRef] = reader.root();
            }
        }
    }

    auto topStructName = boost::to_upper_copy(getExtByResType(resType));
    SchemaStruct schemaStruct;
    schemaStruct.name = topStructName;
    schemaStruct.top = true;
    for (auto &tree : trees) {
        appendGffToSchema(*tree.second, schemaStruct);
    }
    std::vector<std::pair<int, SchemaStruct *>> structs;
    std::stack<std::pair<int, SchemaStruct *>> structStack;
    structStack.push(std::make_pair(0, &schemaStruct));
    while (!structStack.empty()) {
        auto [depth, strct] = structStack.top();
        structStack.pop();

        structs.push_back(std::make_pair(depth, strct));

        for (auto &[_, field] : strct->fields) {
            if (field.subStruct) {
                structStack.push(std::make_pair(depth + 1, field.subStruct.get()));
            }
        }
    }
    std::sort(structs.begin(), structs.end(), [](auto &l, auto &r) {
        return l.first > r.first;
    });

    auto schemaHeaderFilename = getExtByResType(resType) + ".h";
    auto schemaHeaderFile = destDir;
    schemaHeaderFile.append(schemaHeaderFilename);
    writeSchemaHeaderFile(topStructName, structs, schemaHeaderFile);

    auto schemaImplFile = destDir;
    schemaImplFile.append(getExtByResType(resType) + ".cpp");
    writeSchemaImplFile(structs, schemaHeaderFilename, schemaImplFile);
}

} // namespace reone
