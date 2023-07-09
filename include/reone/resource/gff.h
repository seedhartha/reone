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

#pragma once

#include "reone/system/types.h"

namespace reone {

namespace resource {

class Gff : boost::noncopyable {
public:
    enum class FieldType : uint16_t {
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

    struct Field {
        FieldType type {FieldType::Int};
        std::string label;
        std::string strValue; /**< covers CExoString and ResRef */
        glm::vec3 vecValue {0.0f};
        glm::quat quatValue {1.0f, 0.0f, 0.0f, 0.0f};
        ByteBuffer data;
        std::vector<std::shared_ptr<Gff>> children;

        union {
            int32_t intValue;   /**< covers Char, Short, Int and StrRef */
            uint32_t uintValue; /**< covers Byte, Word and Dword */
            int64_t int64Value;
            uint64_t uint64Value {0};
            float floatValue;
            double doubleValue;
        };

        Field() = default;

        Field(FieldType type, std::string label) :
            type(type), label(std::move(label)) {
        }

        std::string toString() const;

        static Field newByte(std::string label, uint32_t val);
        static Field newChar(std::string label, int32_t val);
        static Field newWord(std::string label, uint32_t val);
        static Field newShort(std::string label, int32_t val);
        static Field newDword(std::string label, uint32_t val);
        static Field newInt(std::string label, int32_t val);
        static Field newDword64(std::string label, uint64_t val);
        static Field newInt64(std::string label, int64_t val);
        static Field newFloat(std::string label, float val);
        static Field newDouble(std::string label, double val);
        static Field newCExoString(std::string label, std::string val);
        static Field newResRef(std::string label, std::string val);
        static Field newCExoLocString(std::string label, int32_t strRef, std::string val);
        static Field newVoid(std::string label, ByteBuffer val);
        static Field newStruct(std::string label, std::shared_ptr<Gff> val);
        static Field newList(std::string label, std::vector<std::shared_ptr<Gff>> val);
        static Field newOrientation(std::string label, glm::quat val);
        static Field newVector(std::string label, glm::vec3 val);
        static Field newStrRef(std::string label, int32_t val);
    };

    class Builder {
    public:
        Builder &type(uint32_t type) {
            _type = type;
            return *this;
        }

        Builder &field(Field field) {
            _fields.push_back(std::move(field));
            return *this;
        }

        std::shared_ptr<Gff> build() {
            return std::make_shared<Gff>(_type, _fields);
        }

    private:
        uint32_t _type {0};
        std::vector<Field> _fields;
    };

    Gff(uint32_t type, std::vector<Field> fields) :
        _type(type), _fields(std::move(fields)) {
    }

    bool getBool(const std::string &name, bool defValue = false) const;
    int getInt(const std::string &name, int defValue = 0) const;
    int64_t readInt64(const std::string &name, int64_t defValue = 0) const;
    uint32_t getUint(const std::string &name, uint32_t defValue = 0) const;
    uint64_t readUint64(const std::string &name, uint64_t defValue = 0) const;
    glm::vec3 getColor(const std::string &name, glm::vec3 defValue = glm::vec3(0.0f)) const;
    float getFloat(const std::string &name, float defValue = 0.0f) const;
    double getDouble(const std::string &name, double defValue = 0.0) const;
    std::string getString(const std::string &name, std::string defValue = "") const;
    glm::vec3 getVector(const std::string &name, glm::vec3 defValue = glm::vec3(0.0f)) const;
    glm::quat getOrientation(const std::string &name, glm::quat defValue = glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) const;
    std::shared_ptr<Gff> findStruct(const std::string &name) const;
    std::vector<std::shared_ptr<Gff>> getList(const std::string &name) const;
    ByteBuffer getData(const std::string &name) const;

    uint32_t type() const { return _type; }
    const std::vector<Field> &fields() const { return _fields; }

    template <class T>
    T getEnum(const std::string &name, T defValue) const {
        return static_cast<T>(getInt(name, static_cast<int>(defValue)));
    }

private:
    uint32_t _type {0};
    std::vector<Field> _fields;

    const Field *get(const std::string &name) const;
};

} // namespace resource

} // namespace reone
