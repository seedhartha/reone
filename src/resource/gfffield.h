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

#pragma once

#include "../common/types.h"

namespace reone {

namespace resource {

enum class GffFieldType : uint16_t {
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

struct GffField {
    GffFieldType type {GffFieldType::Int};
    std::string label;
    std::string strValue; /**< covers CExoString and ResRef */
    glm::vec3 vecValue {0.0f};
    glm::quat quatValue {1.0f, 0.0f, 0.0f, 0.0f};
    ByteArray data;
    std::vector<std::shared_ptr<GffStruct>> children;

    union {
        int32_t intValue;   /**< covers Char, Short, Int and StrRef */
        uint32_t uintValue; /**< covers Byte, Word and Dword */
        int64_t int64Value;
        uint64_t uint64Value {0};
        float floatValue;
        double doubleValue;
    };

    GffField() = default;

    GffField(GffFieldType type, std::string label) :
        type(type), label(std::move(label)) {
    }

    static GffField newByte(std::string label, uint32_t val);
    static GffField newChar(std::string label, int32_t val);
    static GffField newWord(std::string label, uint32_t val);
    static GffField newShort(std::string label, int32_t val);
    static GffField newDword(std::string label, uint32_t val);
    static GffField newInt(std::string label, int32_t val);
    static GffField newDword64(std::string label, uint64_t val);
    static GffField newInt64(std::string label, int64_t val);
    static GffField newFloat(std::string label, float val);
    static GffField newDouble(std::string label, double val);
    static GffField newCExoString(std::string label, std::string val);
    static GffField newResRef(std::string label, std::string val);
    static GffField newCExoLocString(std::string label, int32_t strRef, std::string val);
    static GffField newVoid(std::string label, ByteArray val);
    static GffField newStruct(std::string label, std::shared_ptr<GffStruct> val);
    static GffField newList(std::string label, std::vector<std::shared_ptr<GffStruct>> val);
    static GffField newOrientation(std::string label, glm::quat val);
    static GffField newVector(std::string label, glm::vec3 val);
    static GffField newStrRef(std::string label, int32_t val);
};

} // namespace resource

} // namespace reone
