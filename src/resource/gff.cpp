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

#include "gff.h"

#include "../common/logutil.h"

using namespace std;

namespace reone {

namespace resource {

bool Gff::getBool(const string &name, bool defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->intValue != 0;
}

const Gff::Field *Gff::get(const string &name) const {
    auto maybeField = find_if(
        _fields.begin(),
        _fields.end(),
        [&](auto &f) { return f.label == name; });

    return maybeField != _fields.end() ? &*maybeField : nullptr;
}

int Gff::getInt(const string &name, int defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->intValue;
}

int64_t Gff::getInt64(const string &name, int64_t defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->int64Value;
}

uint32_t Gff::getUint(const string &name, uint32_t defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->uintValue;
}

uint64_t Gff::getUint64(const string &name, uint64_t defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->uint64Value;
}

static glm::vec3 colorFromUint32(uint32_t value) {
    glm::vec3 result(
        value & 0xff,
        (value >> 8) & 0xff,
        (value >> 16) & 0xff);

    result /= 255.0f;

    return move(result);
}

glm::vec3 Gff::getColor(const string &name, glm::vec3 defValue) const {
    const Field *field = get(name);
    if (!field)
        return move(defValue);

    return colorFromUint32(field->uintValue);
}

float Gff::getFloat(const string &name, float defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->floatValue;
}

double Gff::getDouble(const string &name, double defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->doubleValue;
}

string Gff::getString(const string &name, string defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->strValue;
}

glm::vec3 Gff::getVector(const string &name, glm::vec3 defValue) const {
    const Field *field = get(name);
    if (!field)
        return move(defValue);

    return field->vecValue;
}

glm::quat Gff::getOrientation(const string &name, glm::quat defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->quatValue;
}

shared_ptr<Gff> Gff::getStruct(const string &name) const {
    const Field *field = get(name);
    if (!field)
        return nullptr;

    return field->children[0];
}

vector<shared_ptr<Gff>> Gff::getList(const string &name) const {
    const Field *field = get(name);
    if (!field)
        return vector<shared_ptr<Gff>>();

    return field->children;
}

ByteArray Gff::getData(const string &name) const {
    const Field *field = get(name);
    if (!field)
        return ByteArray();

    return field->data;
}

string Gff::Field::toString() const {
    switch (type) {
    case FieldType::Byte:
    case FieldType::Word:
    case FieldType::Dword:
        return to_string(uintValue);
    case FieldType::Char:
    case FieldType::Short:
    case FieldType::Int:
        return to_string(intValue);
    case FieldType::Dword64:
        return to_string(uint64Value);
    case FieldType::Int64:
        return to_string(int64Value);
    case FieldType::Float:
        return to_string(floatValue);
    case FieldType::Double:
        return to_string(doubleValue);
    case FieldType::CExoString:
        return strValue;
    case FieldType::ResRef:
        return strValue;
    default:
        throw logic_error("Unsupported field type: " + to_string(static_cast<int>(type)));
    }
}

Gff::Field Gff::Field::newByte(string label, uint32_t val) {
    Field tmp(FieldType::Byte, move(label));
    tmp.uintValue = val;
    return move(tmp);
}

Gff::Field Gff::Field::newChar(string label, int32_t val) {
    Field tmp(FieldType::Char, move(label));
    tmp.intValue = val;
    return move(tmp);
}

Gff::Field Gff::Field::newWord(string label, uint32_t val) {
    Field tmp(FieldType::Word, move(label));
    tmp.uintValue = val;
    return move(tmp);
}

Gff::Field Gff::Field::newShort(string label, int32_t val) {
    Field tmp(FieldType::Short, move(label));
    tmp.intValue = val;
    return move(tmp);
}

Gff::Field Gff::Field::newDword(string label, uint32_t val) {
    Field tmp(FieldType::Dword, move(label));
    tmp.uintValue = val;
    return move(tmp);
}

Gff::Field Gff::Field::newInt(string label, int32_t val) {
    Field tmp(FieldType::Int, move(label));
    tmp.intValue = val;
    return move(tmp);
}

Gff::Field Gff::Field::newDword64(string label, uint64_t val) {
    Field tmp(FieldType::Dword64, move(label));
    tmp.uint64Value = val;
    return move(tmp);
}

Gff::Field Gff::Field::newInt64(string label, int64_t val) {
    Field tmp(FieldType::Int64, move(label));
    tmp.int64Value = val;
    return move(tmp);
}

Gff::Field Gff::Field::newFloat(string label, float val) {
    Field tmp(FieldType::Float, move(label));
    tmp.floatValue = val;
    return move(tmp);
}

Gff::Field Gff::Field::newDouble(string label, double val) {
    Field tmp(FieldType::Double, move(label));
    tmp.doubleValue = val;
    return move(tmp);
}

Gff::Field Gff::Field::newCExoString(string label, string val) {
    Field tmp(FieldType::CExoString, move(label));
    tmp.strValue = move(val);
    return move(tmp);
}

Gff::Field Gff::Field::newResRef(string label, string val) {
    Field tmp(FieldType::ResRef, move(label));
    tmp.strValue = move(val);
    return move(tmp);
}

Gff::Field Gff::Field::newCExoLocString(string label, int32_t strRef, string val) {
    Field tmp(FieldType::CExoLocString, move(label));
    tmp.intValue = strRef;
    tmp.strValue = move(val);
    return move(tmp);
}

Gff::Field Gff::Field::newVoid(string label, ByteArray val) {
    Field tmp(FieldType::Void, move(label));
    tmp.data = move(val);
    return move(tmp);
}

Gff::Field Gff::Field::newStruct(string label, shared_ptr<Gff> val) {
    Field tmp(FieldType::Struct, move(label));
    tmp.children.push_back(move(val));
    return move(tmp);
}

Gff::Field Gff::Field::newList(string label, vector<shared_ptr<Gff>> val) {
    Field tmp(FieldType::List, move(label));
    tmp.children = move(val);
    return move(tmp);
}

Gff::Field Gff::Field::newOrientation(string label, glm::quat val) {
    Field tmp(FieldType::Orientation, move(label));
    tmp.quatValue = move(val);
    return move(tmp);
}

Gff::Field Gff::Field::newVector(string label, glm::vec3 val) {
    Field tmp(FieldType::Vector, move(label));
    tmp.vecValue = move(val);
    return move(tmp);
}

Gff::Field Gff::Field::newStrRef(string label, int32_t val) {
    Field tmp(FieldType::StrRef, move(label));
    tmp.intValue = val;
    return move(tmp);
}

} // namespace resource

} // namespace reone
