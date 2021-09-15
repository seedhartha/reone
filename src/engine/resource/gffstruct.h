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

#include "gfffield.h"

namespace reone {

namespace resource {

class GffStruct : boost::noncopyable {
public:
    GffStruct(uint32_t type) : _type(type) {
    }

    GffStruct(uint32_t type, std::vector<GffField> fields) : _type(type), _fields(std::move(fields)) {
    }

    void add(GffField &&field);

    bool getBool(const std::string &name, bool defValue = false) const;
    int getInt(const std::string &name, int defValue = 0) const;
    uint32_t getUint(const std::string &name, uint32_t defValue = 0) const;
    glm::vec3 getColor(const std::string &name, glm::vec3 defValue = glm::vec3(0.0f)) const;
    float getFloat(const std::string &name, float defValue = 0.0f) const;
    std::string getString(const std::string &name, std::string defValue = "") const;
    glm::vec3 getVector(const std::string &name, glm::vec3 defValue = glm::vec3(0.0f)) const;
    glm::quat getOrientation(const std::string &name, glm::quat defValue = glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) const;
    std::shared_ptr<GffStruct> getStruct(const std::string &name) const;
    std::vector<std::shared_ptr<GffStruct>> getList(const std::string &name) const;

    uint32_t type() const { return _type; }
    const std::vector<GffField> &fields() const { return _fields; }

    template <class T>
    T getEnum(const std::string &name, T defValue) const {
        return static_cast<T>(getInt(name, static_cast<int>(defValue)));
    }

private:
    uint32_t _type { 0 };
    std::vector<GffField> _fields;

    const GffField *get(const std::string &name) const;

    friend class GffReader;
};

} // namespace resource

} // namespace reone
