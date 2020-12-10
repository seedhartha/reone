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

#include "gfffile.h"

#include "glm/gtc/type_ptr.hpp"

#include <boost/format.hpp>

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static const int kSignatureSize = 8;

GffField::GffField(GffFieldType type, const string &label) : _type(type), _label(label) {
}

GffFieldType GffField::type() const {
    return _type;
}

const string &GffField::label() const {
    return _label;
}

const vector<shared_ptr<GffStruct>> &GffField::children() const {
    return _children;
}

bool GffField::asBool() const {
    return _intValue != 0;
}

int64_t GffField::asInt() const {
    return _intValue;
}

uint64_t GffField::asUint() const {
    return _uintValue;
}

float GffField::asFloat() const {
    return _floatValue;
}

double GffField::asDouble() const {
    return _doubleValue;
}

string GffField::asString() const {
    switch (_type) {
        case GffFieldType::CExoString:
        case GffFieldType::ResRef:
            return _strValue;

        case GffFieldType::Char:
            return string(1, _strValue[0]);

        case GffFieldType::Short:
        case GffFieldType::Int:
        case GffFieldType::Int64:
        case GffFieldType::CExoLocString:
        case GffFieldType::StrRef:
            return to_string(_intValue);

        case GffFieldType::Byte:
        case GffFieldType::Word:
        case GffFieldType::Dword:
        case GffFieldType::Dword64:
            return to_string(_uintValue);

        case GffFieldType::Float:
            return to_string(_floatValue);

        case GffFieldType::Double:
            return to_string(_doubleValue);

        case GffFieldType::Void:
            return boost::str(boost::format("[array of %d bytes]") % _data.size());

        default:
            throw logic_error("GFF: field type cannot be converted to string: " + to_string(static_cast<int>(_type)));
    }
}

const ByteArray &GffField::asByteArray() const {
    return _data;
}

vector<float> GffField::asFloatArray() const {
    vector<float> values(_data.size() / sizeof(float));
    memcpy(&values[0], &_data[0], values.size() * sizeof(float));
    return move(values);
}

const GffStruct &GffField::asStruct() const {
    return *_children[0];
}

glm::vec3 GffField::asVector() const {
    const float *values = reinterpret_cast<const float *>(&_data[0]);
    return glm::vec3(values[0], values[1], values[2]);
}

glm::quat GffField::asOrientation() const {
    const float *values = reinterpret_cast<const float *>(&_data[0]);
    return glm::quat(values[0], values[1], values[2], values[3]);
}

GffStruct::GffStruct(GffFieldType type) : _type(type) {
}

const vector<GffField> &GffStruct::fields() const {
    return _fields;
}

const GffField *GffStruct::find(const string &name) const {
    auto it = find_if(
        _fields.begin(),
        _fields.end(),
        [&](const GffField &f) { return f.label() == name; });

    return it != _fields.end() ? &*it : nullptr;
}

bool GffStruct::getBool(const string &name, bool defaultValue) const {
    const GffField *field = find(name);
    return field ? field->asBool() : defaultValue;
}

int GffStruct::getInt(const string &name, int defaultValue) const {
    const GffField *field = find(name);
    return field ? static_cast<int>(field->asInt()) : defaultValue;
}

float GffStruct::getFloat(const string &name, float defaultValue) const {
    const GffField *field = find(name);
    return field ? field->asFloat() : defaultValue;
}

string GffStruct::getString(const string &name, const char *defaultValue) const {
    const GffField *field = find(name);
    return field ? field->asString() : defaultValue;
}

glm::vec3 GffStruct::getVector(const string &name, glm::vec3 defaultValue) const {
    const GffField *field = find(name);
    return field ? field->asVector() : move(defaultValue);
}

glm::quat GffStruct::getOrientation(const string &name, glm::quat defaultValue) const {
    const GffField *field = find(name);
    return field ? field->asOrientation() : move(defaultValue);
}

shared_ptr<GffStruct> GffStruct::getStruct(const string &name) const {
    const GffField *field = find(name);
    return field ? field->children()[0] : nullptr;
}

vector<shared_ptr<GffStruct>> GffStruct::getList(const string &name) const {
    const GffField *field = find(name);
    return field ? field->children() : vector<shared_ptr<GffStruct>>();
}

GffFile::GffFile() : BinaryFile(kSignatureSize) {
}

void GffFile::doLoad() {
    _structOffset = readUint32();
    _structCount = readUint32();
    _fieldOffset = readUint32();
    _fieldCount = readUint32();
    _labelOffset = readUint32();
    _labelCount = readUint32();
    _fieldDataOffset = readUint32();
    _fieldDataCount = readUint32();
    _fieldIndicesOffset = readUint32();
    _fieldIncidesCount = readUint32();
    _listIndicesOffset = readUint32();
    _listIndicesCount = readUint32();

    _top = move(readStruct(0));
}

shared_ptr<GffStruct> GffFile::top() const {
    return _top;
}

unique_ptr<GffStruct> GffFile::readStruct(int idx) {
    seek(_structOffset + 12 * idx);

    uint32_t type = readUint32();
    uint32_t dataOrDataOffset = readUint32();
    uint32_t fieldCount = readUint32();

    auto gffs = make_unique<GffStruct>(static_cast<GffFieldType>(type));

    if (fieldCount == 1) {
        gffs->_fields.push_back(readField(dataOrDataOffset));
    } else {
        vector<uint32_t> indices(readFieldIndices(dataOrDataOffset, fieldCount));
        for (auto &idx : indices) {
            gffs->_fields.push_back(readField(idx));
        }
    }

    return move(gffs);
}

GffField GffFile::readField(int idx) {
    seek(_fieldOffset + 12 * idx);

    uint32_t type = readUint32();
    uint32_t labelIndex = readUint32();
    uint32_t dataOrDataOffset = readUint32();

    string label(readLabel(labelIndex));
    GffField field(static_cast<GffFieldType>(type), label);
    LocString locString;
    vector<uint32_t> list;

    switch (field._type) {
        case GffFieldType::Byte:
        case GffFieldType::Char:
        case GffFieldType::Word:
        case GffFieldType::Short:
        case GffFieldType::Dword:
        case GffFieldType::Int:
        case GffFieldType::Float:
            field._uintValue = dataOrDataOffset;
            break;

        case GffFieldType::Dword64:
        case GffFieldType::Int64:
        case GffFieldType::Double:
            field._uintValue = readQWordFieldData(dataOrDataOffset);
            break;

        case GffFieldType::CExoString:
            field._strValue = readStringFieldData(dataOrDataOffset);
            break;

        case GffFieldType::ResRef:
            field._strValue = readResRefFieldData(dataOrDataOffset);
            break;

        case GffFieldType::CExoLocString:
            locString = readCExoLocStringFieldData(dataOrDataOffset);
            field._intValue = locString.strRef;
            field._strValue = locString.subString;
            break;

        case GffFieldType::StrRef:
            field._intValue = readStrRefFieldData(dataOrDataOffset);
            break;

        case GffFieldType::Void:
            field._data = readByteArrayFieldData(dataOrDataOffset);
            break;

        case GffFieldType::Orientation:
            field._data = readByteArrayFieldData(dataOrDataOffset, 4 * sizeof(float));
            break;

        case GffFieldType::Vector:
            field._data = readByteArrayFieldData(dataOrDataOffset, 3 * sizeof(float));
            break;

        case GffFieldType::Struct:
            field._children.push_back(readStruct(dataOrDataOffset));
            break;

        case GffFieldType::List:
            list = readList(dataOrDataOffset);
            for (auto &item : list) {
                field._children.push_back(readStruct(item));
            }
            break;

        default:
            throw runtime_error("GFF: unsupported field type: " + to_string(type));
    }

    return move(field);
}

string GffFile::readLabel(int idx) {
    uint32_t off = _labelOffset + 16 * idx;
    return readCString(off, 16);
}

vector<uint32_t> GffFile::readFieldIndices(uint32_t off, int count) {
    return readArray<uint32_t>(_fieldIndicesOffset + off, count);
}

uint64_t GffFile::readQWordFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);
    uint64_t val = readUint64();
    seek(pos);

    return val;
}

string GffFile::readStringFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint32_t size = readUint32();
    string s(readCString(size));
    seek(pos);

    return move(s);
}

string GffFile::readResRefFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint8_t size = readByte();
    string s(readCString(size));
    seek(pos);

    return move(s);
}

GffFile::LocString GffFile::readCExoLocStringFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint32_t size = readUint32();
    int32_t ref = readInt32();
    uint32_t count = readUint32();

    LocString loc;
    loc.strRef = ref;

    if (count > 0) {
        int32_t type = readInt32();
        uint32_t ssSize = readUint32();
        loc.subString = readCString(ssSize);
    }

    seek(pos);

    return move(loc);
}

int32_t GffFile::readStrRefFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint32_t size = readUint32();
    int32_t ref = readUint32();

    seek(pos);

    return ref;
}

ByteArray GffFile::readByteArrayFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint32_t size = readUint32();
    ByteArray arr(readArray<char>(size));
    seek(pos);

    return move(arr);
}

ByteArray GffFile::readByteArrayFieldData(uint32_t off, int size) {
    return readArray<char>(_fieldDataOffset + off, size);
}

vector<uint32_t> GffFile::readList(uint32_t off) {
    size_t pos = tell();
    seek(static_cast<size_t>(_listIndicesOffset) + off);

    uint32_t count = readUint32();
    vector<uint32_t> arr(readArray<uint32_t>(count));
    seek(pos);

    return move(arr);
}

} // namespace resource

} // namespace reone
