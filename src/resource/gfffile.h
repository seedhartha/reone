/*
 * Copyright (c) 2020 The reone project contributors
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

#pragma once

#include "binfile.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"

namespace reone {

namespace resource {

enum class GffFieldType {
    Byte = 0,
    Char = 1,
    Word = 2,
    Short = 3,
    Dword = 4,
    Int = 5,
    Dword64 = 6,
    Int64 = 7,
    Float = 8,
    Double = 9,
    CExoString = 10,
    ResRef = 11,
    CExoLocString = 12,
    Void = 13,
    Struct = 14,
    List = 15,
    Orientation = 16,
    Vector = 17,
    StrRef = 18
};

class GffStruct;

class GffField {
public:
    GffField(GffFieldType type, const std::string &label);
    GffField(GffField &&) = default;

    GffField &operator=(GffField &&) = default;

    GffFieldType type() const;
    const std::string &label() const;
    const std::vector<GffStruct> &children() const;
    bool asBool() const;
    int64_t asInt() const;
    uint64_t asUint() const;
    float asFloat() const;
    double asDouble() const;
    std::string asString() const;
    const ByteArray &asByteArray() const;
    std::vector<float> asFloatArray() const;
    const GffStruct &asStruct() const;
    glm::vec3 asVector() const;
    glm::quat asOrientation() const;

private:
    GffFieldType _type { GffFieldType::Byte };
    std::string _label;
    std::vector<GffStruct> _children;
    std::string _strValue;
    ByteArray _data;

    union {
        int64_t _intValue;
        uint64_t _uintValue;
        float _floatValue;
        double _doubleValue;
    };

    GffField(const GffField &) = delete;
    GffField &operator=(const GffField &) = delete;

    friend class GffFile;
};

class GffStruct {
public:
    GffStruct(GffFieldType type);
    GffStruct(GffStruct &&) = default;

    GffStruct &operator=(GffStruct &&) = default;

    void add(GffField &&field);
    const GffField *find(const std::string &name) const;

    void setType(GffFieldType type);

    bool getBool(const std::string &name, bool defaultValue = false) const;
    int getInt(const std::string &name) const;
    int getInt(const std::string &name, int defaultValue) const;
    float getFloat(const std::string &name) const;
    float getFloat(const std::string &name, float defaultValue) const;
    std::string getString(const std::string &name) const;
    const GffStruct &getStruct(const std::string &name) const;
    const std::vector<GffStruct> &getList(const std::string &name) const;
    glm::vec3 getVector(const std::string &name) const;
    glm::quat getOrientation(const std::string &name) const;

    const std::vector<GffField> &fields() const;

private:
    GffFieldType _type { GffFieldType::Byte };
    std::vector<GffField> _fields;

    GffStruct(const GffStruct &) = delete;
    GffStruct &operator=(const GffStruct &) = delete;
};

class GffFile : public BinaryFile {
public:
    GffFile();
    std::shared_ptr<GffStruct> top() const;

private:
    struct LocString {
        int32_t strRef { -1 };
        std::string subString;
    };

    uint32_t _structOffset { 0 };
    int _structCount { 0 };
    uint32_t _fieldOffset { 0 };
    int _fieldCount { 0 };
    uint32_t _labelOffset { 0 };
    int _labelCount { 0 };
    uint32_t _fieldDataOffset { 0 };
    int _fieldDataCount { 0 };
    uint32_t _fieldIndicesOffset { 0 };
    int _fieldIncidesCount { 0 };
    uint32_t _listIndicesOffset { 0 };
    int _listIndicesCount { 0 };
    std::shared_ptr<GffStruct> _top;

    void doLoad() override;
    GffStruct readStruct(int idx);
    GffField readField(int idx);
    std::string readLabel(int idx);
    std::vector<uint32_t> readFieldIndices(uint32_t off, int count);
    uint64_t readQWordFieldData(uint32_t off);
    std::string readStringFieldData(uint32_t off);
    std::string readResRefFieldData(uint32_t off);
    LocString readCExoLocStringFieldData(uint32_t off);
    int32_t readStrRefFieldData(uint32_t off);
    ByteArray readByteArrayFieldData(uint32_t off);
    ByteArray readByteArrayFieldData(uint32_t off, int size);
    std::vector<uint32_t> readList(uint32_t off);
};

} // namespace resource

} // namespace reone
