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

#include "reone/resource/gff.h"

#include "reone/resource/exception/format.h"
#include "reone/system/logutil.h"

namespace reone {

namespace resource {

bool Gff::getBool(const std::string &name, bool defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->intValue != 0;
}

const Gff::Field *Gff::get(const std::string &name) const {
    auto maybeField = std::find_if(
        _fields.begin(),
        _fields.end(),
        [&](auto &f) { return f.label == name; });

    return maybeField != _fields.end() ? &*maybeField : nullptr;
}

int Gff::getInt(const std::string &name, int defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->intValue;
}

int64_t Gff::readInt64(const std::string &name, int64_t defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->int64Value;
}

uint32_t Gff::getUint(const std::string &name, uint32_t defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->uintValue;
}

uint64_t Gff::readUint64(const std::string &name, uint64_t defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->uint64Value;
}

glm::vec3 Gff::getColor(const std::string &name, glm::vec3 defValue) const {
    const Field *field = get(name);
    if (!field)
        return std::move(defValue);

    return colorFromUint32(field->uintValue);
}

float Gff::getFloat(const std::string &name, float defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->floatValue;
}

double Gff::getDouble(const std::string &name, double defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->doubleValue;
}

std::string Gff::getString(const std::string &name, std::string defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->strValue;
}

glm::vec3 Gff::getVector(const std::string &name, glm::vec3 defValue) const {
    const Field *field = get(name);
    if (!field)
        return std::move(defValue);

    return field->vecValue;
}

glm::quat Gff::getOrientation(const std::string &name, glm::quat defValue) const {
    const Field *field = get(name);
    if (!field)
        return defValue;

    return field->quatValue;
}

std::shared_ptr<Gff> Gff::findStruct(const std::string &name) const {
    const Field *field = get(name);
    if (!field)
        return nullptr;

    return field->children[0];
}

std::vector<std::shared_ptr<Gff>> Gff::getList(const std::string &name) const {
    const Field *field = get(name);
    if (!field)
        return std::vector<std::shared_ptr<Gff>>();

    return field->children;
}

ByteBuffer Gff::getData(const std::string &name) const {
    const Field *field = get(name);
    if (!field)
        return ByteBuffer();

    return field->data;
}

std::string Gff::Field::toString() const {
    switch (type) {
    case FieldType::Byte:
    case FieldType::Word:
    case FieldType::Dword:
        return std::to_string(uintValue);
    case FieldType::Char:
    case FieldType::Short:
    case FieldType::Int:
        return std::to_string(intValue);
    case FieldType::Dword64:
        return std::to_string(uint64Value);
    case FieldType::Int64:
        return std::to_string(int64Value);
    case FieldType::Float:
        return std::to_string(floatValue);
    case FieldType::Double:
        return std::to_string(doubleValue);
    case FieldType::CExoString:
    case FieldType::ResRef:
        return strValue;
    default:
        throw FormatException("Unsupported field type: " + std::to_string(static_cast<int>(type)));
    }
}

Gff::Field Gff::Field::newByte(std::string label, uint32_t val) {
    Field tmp(FieldType::Byte, std::move(label));
    tmp.uintValue = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newChar(std::string label, int32_t val) {
    Field tmp(FieldType::Char, std::move(label));
    tmp.intValue = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newWord(std::string label, uint32_t val) {
    Field tmp(FieldType::Word, std::move(label));
    tmp.uintValue = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newShort(std::string label, int32_t val) {
    Field tmp(FieldType::Short, std::move(label));
    tmp.intValue = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newDword(std::string label, uint32_t val) {
    Field tmp(FieldType::Dword, std::move(label));
    tmp.uintValue = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newInt(std::string label, int32_t val) {
    Field tmp(FieldType::Int, std::move(label));
    tmp.intValue = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newDword64(std::string label, uint64_t val) {
    Field tmp(FieldType::Dword64, std::move(label));
    tmp.uint64Value = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newInt64(std::string label, int64_t val) {
    Field tmp(FieldType::Int64, std::move(label));
    tmp.int64Value = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newFloat(std::string label, float val) {
    Field tmp(FieldType::Float, std::move(label));
    tmp.floatValue = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newDouble(std::string label, double val) {
    Field tmp(FieldType::Double, std::move(label));
    tmp.doubleValue = val;
    return std::move(tmp);
}

Gff::Field Gff::Field::newCExoString(std::string label, std::string val) {
    Field tmp(FieldType::CExoString, std::move(label));
    tmp.strValue = std::move(val);
    return std::move(tmp);
}

Gff::Field Gff::Field::newResRef(std::string label, std::string val) {
    Field tmp(FieldType::ResRef, std::move(label));
    tmp.strValue = std::move(val);
    return std::move(tmp);
}

Gff::Field Gff::Field::newCExoLocString(std::string label, int32_t strRef, std::string val) {
    Field tmp(FieldType::CExoLocString, std::move(label));
    tmp.intValue = strRef;
    tmp.strValue = std::move(val);
    return std::move(tmp);
}

Gff::Field Gff::Field::newVoid(std::string label, ByteBuffer val) {
    Field tmp(FieldType::Void, std::move(label));
    tmp.data = std::move(val);
    return std::move(tmp);
}

Gff::Field Gff::Field::newStruct(std::string label, std::shared_ptr<Gff> val) {
    Field tmp(FieldType::Struct, std::move(label));
    tmp.children.push_back(std::move(val));
    return std::move(tmp);
}

Gff::Field Gff::Field::newList(std::string label, std::vector<std::shared_ptr<Gff>> val) {
    Field tmp(FieldType::List, std::move(label));
    tmp.children = std::move(val);
    return std::move(tmp);
}

Gff::Field Gff::Field::newOrientation(std::string label, glm::quat val) {
    Field tmp(FieldType::Orientation, std::move(label));
    tmp.quatValue = std::move(val);
    return std::move(tmp);
}

Gff::Field Gff::Field::newVector(std::string label, glm::vec3 val) {
    Field tmp(FieldType::Vector, std::move(label));
    tmp.vecValue = std::move(val);
    return std::move(tmp);
}

Gff::Field Gff::Field::newStrRef(std::string label, int32_t val) {
    Field tmp(FieldType::StrRef, std::move(label));
    tmp.intValue = val;
    return std::move(tmp);
}

} // namespace resource

} // namespace reone
