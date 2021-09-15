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

#include "gfffield.h"

using namespace std;

namespace reone {

namespace resource {

GffField GffField::newByte(string label, uint32_t val) {
    GffField tmp(GffFieldType::Byte, move(label));
    tmp.uintValue = val;
    return move(tmp);
}

GffField GffField::newChar(string label, int32_t val) {
    GffField tmp(GffFieldType::Char, move(label));
    tmp.intValue = val;
    return move(tmp);
}

GffField GffField::newWord(string label, uint32_t val) {
    GffField tmp(GffFieldType::Word, move(label));
    tmp.uintValue = val;
    return move(tmp);
}

GffField GffField::newShort(string label, int32_t val) {
    GffField tmp(GffFieldType::Short, move(label));
    tmp.intValue = val;
    return move(tmp);
}

GffField GffField::newDword(string label, uint32_t val) {
    GffField tmp(GffFieldType::Dword, move(label));
    tmp.uintValue = val;
    return move(tmp);
}

GffField GffField::newInt(string label, int32_t val) {
    GffField tmp(GffFieldType::Int, move(label));
    tmp.intValue = val;
    return move(tmp);
}

GffField GffField::newDword64(string label, uint64_t val) {
    GffField tmp(GffFieldType::Dword64, move(label));
    tmp.uint64Value = val;
    return move(tmp);
}

GffField GffField::newInt64(string label, int64_t val) {
    GffField tmp(GffFieldType::Int64, move(label));
    tmp.int64Value = val;
    return move(tmp);
}

GffField GffField::newFloat(string label, float val) {
    GffField tmp(GffFieldType::Float, move(label));
    tmp.floatValue = val;
    return move(tmp);
}

GffField GffField::newDouble(string label, double val) {
    GffField tmp(GffFieldType::Double, move(label));
    tmp.doubleValue = val;
    return move(tmp);
}

GffField GffField::newCExoString(string label, string val) {
    GffField tmp(GffFieldType::CExoString, move(label));
    tmp.strValue = move(val);
    return move(tmp);
}

GffField GffField::newResRef(string label, string val) {
    GffField tmp(GffFieldType::ResRef, move(label));
    tmp.strValue = move(val);
    return move(tmp);
}

GffField GffField::newCExoLocString(string label, int32_t strRef, string val) {
    GffField tmp(GffFieldType::CExoLocString, move(label));
    tmp.intValue = strRef;
    tmp.strValue = move(val);
    return move(tmp);
}

GffField GffField::newVoid(string label, ByteArray val) {
    GffField tmp(GffFieldType::Void, move(label));
    tmp.data = move(val);
    return move(tmp);
}

GffField GffField::newStruct(string label, shared_ptr<GffStruct> val) {
    GffField tmp(GffFieldType::Struct, move(label));
    tmp.children.push_back(move(val));
    return move(tmp);
}

GffField GffField::newList(string label, vector<shared_ptr<GffStruct>> val) {
    GffField tmp(GffFieldType::List, move(label));
    tmp.children = move(val);
    return move(tmp);
}

GffField GffField::newOrientation(string label, glm::quat val) {
    GffField tmp(GffFieldType::Orientation, move(label));
    tmp.quatValue = move(val);
    return move(tmp);
}

GffField GffField::newVector(string label, glm::vec3 val) {
    GffField tmp(GffFieldType::Vector, move(label));
    tmp.vecValue = move(val);
    return move(tmp);
}

GffField GffField::newStrRef(string label, int32_t val) {
    GffField tmp(GffFieldType::StrRef, move(label));
    tmp.intValue = val;
    return move(tmp);
}

} // namespace resource

} // namespace reone
