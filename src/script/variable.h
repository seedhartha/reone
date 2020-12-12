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

#include <memory>
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

class EngineType;
class ScriptObject;

struct Variable {
    VariableType type { VariableType::Void };
    std::string strValue;
    glm::vec3 vecValue;
    std::shared_ptr<ScriptObject> object;
    std::shared_ptr<EngineType> engineType;
    ExecutionContext context;

    union {
        int intValue { 0 };
        float floatValue;
    };

    Variable() = default;
    Variable(int value);
    Variable(float value);
    Variable(std::string value);
    Variable(glm::vec3 value);
    Variable(const std::shared_ptr<ScriptObject> &object);
    Variable(VariableType type, const std::shared_ptr<EngineType> &engineType);
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
