/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <string>

#include "glm/vec3.hpp"

#include "types.h"

namespace reone {

namespace script {

enum class VariableType {
    Void,
    Int,
    Float,
    String,
    Object,
    Vector,
    Effect,
    Event,
    Location,
    Talent,
    Action
};

struct Variable {
    VariableType type { VariableType::Void };
    std::string strValue;
    glm::vec3 vecValue { 0.0f };
    ExecutionContext context;

    union {
        int intValue { 0 };
        float floatValue;
        int objectId;
        int engineTypeId;
    };

    Variable() = default;
    Variable(VariableType type);
    Variable(int value);
    Variable(float value);
    Variable(const std::string &value);
    Variable(const glm::vec3 &value);
    Variable(const ExecutionContext &context);

    Variable operator+(const Variable &other) const;
    Variable operator-(const Variable &other) const;
    Variable operator*(const Variable &other) const;
    Variable operator/(const Variable &other) const;

    bool operator==(const Variable &other) const;
    bool operator!=(const Variable &other) const;
    bool operator<(const Variable &other) const;
    bool operator<=(const Variable &other) const;
    bool operator>(const Variable &other) const;
    bool operator>=(const Variable &other) const;

    const std::string toString() const;
};

} // namespace script

} // namespae reone
