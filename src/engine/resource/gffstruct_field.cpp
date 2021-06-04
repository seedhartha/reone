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

#include "gffstruct.h"

using namespace std;

namespace reone {

namespace resource {

GffStruct::Field::Field(FieldType type, string label) : type(type), label(move(label)) {
}

GffStruct::Field GffStruct::Field::newByte(string label, uint32_t val) {
    GffStruct::Field tmp(GffStruct::FieldType::Byte, move(label));
    tmp.uintValue = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newChar(string label, int32_t val) {
    GffStruct::Field tmp(GffStruct::FieldType::Char, move(label));
    tmp.intValue = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newWord(string label, uint32_t val) {
    GffStruct::Field tmp(GffStruct::FieldType::Word, move(label));
    tmp.uintValue = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newShort(string label, int32_t val) {
    GffStruct::Field tmp(GffStruct::FieldType::Short, move(label));
    tmp.intValue = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newDword(string label, uint32_t val) {
    GffStruct::Field tmp(GffStruct::FieldType::Dword, move(label));
    tmp.uintValue = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newInt(string label, int32_t val) {
    GffStruct::Field tmp(GffStruct::FieldType::Int, move(label));
    tmp.intValue = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newDword64(string label, uint64_t val) {
    GffStruct::Field tmp(GffStruct::FieldType::Dword64, move(label));
    tmp.uint64Value = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newInt64(string label, int64_t val) {
    GffStruct::Field tmp(GffStruct::FieldType::Int64, move(label));
    tmp.int64Value = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newFloat(string label, float val) {
    GffStruct::Field tmp(GffStruct::FieldType::Float, move(label));
    tmp.floatValue = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newDouble(string label, double val) {
    GffStruct::Field tmp(GffStruct::FieldType::Double, move(label));
    tmp.doubleValue = val;
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newCExoString(string label, string val) {
    GffStruct::Field tmp(GffStruct::FieldType::CExoString, move(label));
    tmp.strValue = move(val);
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newResRef(string label, string val) {
    GffStruct::Field tmp(GffStruct::FieldType::ResRef, move(label));
    tmp.strValue = move(val);
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newCExoLocString(string label, int32_t strRef, string val) {
    GffStruct::Field tmp(GffStruct::FieldType::CExoLocString, move(label));
    tmp.intValue = strRef;
    tmp.strValue = move(val);
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newVoid(string label, ByteArray val) {
    GffStruct::Field tmp(GffStruct::FieldType::Void, move(label));
    tmp.data = move(val);
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newStruct(string label, shared_ptr<GffStruct> val) {
    GffStruct::Field tmp(GffStruct::FieldType::Struct, move(label));
    tmp.children.push_back(move(val));
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newList(string label, vector<shared_ptr<GffStruct>> val) {
    GffStruct::Field tmp(GffStruct::FieldType::List, move(label));
    tmp.children = move(val);
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newOrientation(string label, glm::quat val) {
    GffStruct::Field tmp(GffStruct::FieldType::Orientation, move(label));
    tmp.quatValue = move(val);
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newVector(string label, glm::vec3 val) {
    GffStruct::Field tmp(GffStruct::FieldType::Vector, move(label));
    tmp.vecValue = move(val);
    return move(tmp);
}

GffStruct::Field GffStruct::Field::newStrRef(string label, int32_t val) {
    GffStruct::Field tmp(GffStruct::FieldType::StrRef, move(label));
    tmp.intValue = val;
    return move(tmp);
}

} // namespace resource

} // namespace reone
