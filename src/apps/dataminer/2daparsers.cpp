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

#include "2daparsers.h"

#include "reone/resource/container/keybif.h"
#include "reone/resource/format/2dareader.h"
#include "reone/system/checkutil.h"
#include "reone/system/fileutil.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/textwriter.h"

#include "code.h"

using namespace reone::resource;

namespace reone {

enum class TwoDAColumnType {
    Unknown,
    Bool,
    Int,
    HexInt,
    Float,
    String
};

struct TwoDAColumn {
    std::string name;
    TwoDAColumnType type {TwoDAColumnType::Unknown};
    bool optional {false};
};

static std::string sanitizeColumnName(const std::string &name) {
    std::string sanitized {name};
    static std::set<std::string> reserved {"default"};
    if (std::isdigit(name[0]) || reserved.count(name) > 0) {
        sanitized.insert(0, "_");
    }
    return sanitized;
}

static TwoDAColumnType guessColumnType(const std::string &value) {
    std::smatch match;
    std::regex boolRegEx {"^[01]$"};
    if (std::regex_match(value, match, boolRegEx)) {
        return TwoDAColumnType::Bool;
    }
    std::regex intRegEx {"^\\d+$"};
    if (std::regex_match(value, match, intRegEx)) {
        return TwoDAColumnType::Int;
    }
    std::regex hexIntRegEx {"^0x\\d+$"};
    if (std::regex_match(value, match, hexIntRegEx)) {
        return TwoDAColumnType::HexInt;
    }
    std::regex floatRegEx {"^\\d+\\.\\d+$"};
    if (std::regex_match(value, match, floatRegEx)) {
        return TwoDAColumnType::Float;
    }
    return TwoDAColumnType::String;
}

static TwoDAColumnType reduceColumnType(TwoDAColumnType curtype, TwoDAColumnType newtype) {
    if (curtype == newtype || curtype == TwoDAColumnType::Unknown) {
        return newtype;
    }
    if (curtype == TwoDAColumnType::String || newtype == TwoDAColumnType::String) {
        return TwoDAColumnType::String;
    }
    std::set<TwoDAColumnType> floatConvertibleTypes {TwoDAColumnType::Bool, TwoDAColumnType::Int};
    if ((curtype == TwoDAColumnType::Float && floatConvertibleTypes.count(newtype) > 0) ||
        (newtype == TwoDAColumnType::Float && floatConvertibleTypes.count(curtype) > 0)) {
        return TwoDAColumnType::Float;
    }
    if ((curtype == TwoDAColumnType::Int && newtype == TwoDAColumnType::Bool) ||
        (newtype == TwoDAColumnType::Int && curtype == TwoDAColumnType::Bool)) {
        return TwoDAColumnType::Int;
    }
    throw std::logic_error(str(boost::format("Unable to reduce column type: curtype=%d newtype=%d") % static_cast<int>(curtype) % static_cast<int>(newtype)));
}

static std::string cppTypeFromColumnType(TwoDAColumnType type) {
    switch (type) {
    case TwoDAColumnType::Bool:
        return "bool";
    case TwoDAColumnType::Int:
        return "int";
    case TwoDAColumnType::HexInt:
        return "uint32_t";
    case TwoDAColumnType::Float:
        return "float";
    case TwoDAColumnType::Unknown:
    case TwoDAColumnType::String:
        return "std::string";
    default:
        throw std::invalid_argument("Unexpected column type: " + std::to_string(static_cast<int>(type)));
    }
}

static void writeParserHeader(const ResRef &resRef,
                              const std::vector<TwoDAColumn> &columns,
                              const std::filesystem::path &destdir) {
    auto hPath = destdir;
    hPath /= resRef.value() + ".h";
    FileOutputStream stream {hPath};
    TextWriter writer {stream};
    writer.writeLine(kCopyrightNotice);
    writer.writeLine("");
    writer.writeLine("#pragma once");
    writer.writeLine("");
    writer.writeLine(str(boost::format(kIncludeFormat) % "reone/resource/2da.h"));
    writer.writeLine("");
    writer.writeLine("namespace reone {");
    writer.writeLine("");
    writer.writeLine("namespace resource {");
    writer.writeLine("");
    writer.writeLine("namespace generated {");
    writer.writeLine("");
    writer.writeLine("struct " + resRef.value() + " {");
    for (const auto &column : columns) {
        auto cppType = cppTypeFromColumnType(column.type);
        if (column.optional) {
            cppType = str(boost::format("std::optional<%s>") % cppType);
        }
        writer.writeLine(str(boost::format("%s%s %s;") % kIndent % cppType % column.name));
    }
    writer.writeLine("};");
    writer.writeLine("");
    writer.writeLine(str(boost::format("%1% parse_%1%(const TwoDA &twoDA, int row) {") % resRef.value()));
    writer.writeLine(str(boost::format("%s%s strct;") % kIndent % resRef.value()));
    for (const auto &column : columns) {
        std::string getMethod;
        switch (column.type) {
        case TwoDAColumnType::Bool:
            getMethod = "Bool";
            break;
        case TwoDAColumnType::Int:
            getMethod = "Int";
            break;
        case TwoDAColumnType::HexInt:
            getMethod = "HexInt";
            break;
        case TwoDAColumnType::Float:
            getMethod = "Float";
            break;
        case TwoDAColumnType::Unknown:
        case TwoDAColumnType::String:
            getMethod = "String";
            break;
        default:
            throw std::logic_error("Unexpected column type: " + std::to_string(static_cast<int>(column.type)));
        }
        if (column.optional) {
            getMethod.append("Opt");
        }
        writer.writeLine(str(boost::format("%1%strct.%2% = twoDA.get%3%(row, \"%2%\");") % kIndent % column.name % getMethod));
    }
    writer.writeLine(str(boost::format("%sreturn strct;") % kIndent));
    writer.writeLine("}");
    writer.writeLine("");
    writer.writeLine("} // namespace generated");
    writer.writeLine("");
    writer.writeLine("} // namespace resource");
    writer.writeLine("");
    writer.writeLine("} // namespace reone");
    writer.writeLine("");
}

void generate2DAParsers(const std::filesystem::path &k1dir,
                        const std::filesystem::path &k2dir,
                        const std::filesystem::path &destdir) {
    std::vector<std::pair<ResRef, std::shared_ptr<TwoDA>>> twoDAs;

    auto k1KeyPath = findFileIgnoreCase(k1dir, "chitin.key");
    if (k1KeyPath) {
        KeyBifResourceContainer k1KeyBif {*k1KeyPath};
        k1KeyBif.init();
        for (const auto &resId : k1KeyBif.resourceIds()) {
            if (resId.type != ResType::TwoDA) {
                continue;
            }
            auto data = k1KeyBif.findResourceData(resId);
            if (!data) {
                continue;
            }
            MemoryInputStream stream {*data};
            TwoDAReader reader {stream};
            try {
                reader.load();
            } catch (const std::exception &ex) {
                std::cerr << "Error loading 2DA: " << resId.resRef.value() << ": " << ex.what() << std::endl;
            }
            twoDAs.push_back({resId.resRef, reader.twoDA()});
        }
    }

    auto k2KeyPath = findFileIgnoreCase(k2dir, "chitin.key");
    if (k2KeyPath) {
        KeyBifResourceContainer k2KeyBif {*k2KeyPath};
        k2KeyBif.init();
        for (const auto &resId : k2KeyBif.resourceIds()) {
            if (resId.type != ResType::TwoDA) {
                continue;
            }
            auto data = k2KeyBif.findResourceData(resId);
            if (!data) {
                continue;
            }
            MemoryInputStream stream {*data};
            TwoDAReader reader {stream};
            try {
                reader.load();
            } catch (const std::exception &ex) {
                std::cerr << "Error loading 2DA: " << resId.resRef.value() << ": " << ex.what() << std::endl;
            }
            twoDAs.push_back({resId.resRef, reader.twoDA()});
        }
    }

    std::map<ResRef, std::map<std::string, TwoDAColumn>> resRefToColumns;
    for (const auto &[resRef, twoDA] : twoDAs) {
        std::map<std::string, TwoDAColumn> nameToColumn;
        if (resRefToColumns.count(resRef) > 0) {
            nameToColumn = resRefToColumns.at(resRef);
        }
        for (const auto &colName : twoDA->columns()) {
            if (nameToColumn.count(colName) == 0) {
                TwoDAColumn column;
                column.name = colName;
                nameToColumn.insert({colName, std::move(column)});
            }
        }
        if (twoDA->getRowCount() == 0) {
            continue;
        }
        for (const auto &row : twoDA->rows()) {
            for (size_t col = 0; col < twoDA->getColumnCount(); ++col) {
                auto &column = nameToColumn.at(twoDA->columns()[col]);
                checkGreater("row values count", row.values.size(), col);
                const auto &value = row.values[col];
                if (value.empty()) {
                    column.optional = true;
                    continue;
                }
                column.type = reduceColumnType(column.type, guessColumnType(value));
            }
        }
        std::vector<TwoDAColumn> columns;
        for (const auto &[name, column] : nameToColumn) {
            TwoDAColumn copy {column};
            copy.name = sanitizeColumnName(name);
            columns.push_back(std::move(copy));
        }
        writeParserHeader(resRef, columns, destdir);
    }
}

} // namespace reone
