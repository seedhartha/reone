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

#include "types.h"

namespace reone {

namespace script {

enum class VariableType {
    Void,
    Int,
    Float,
    String,
    Vector,
    Object,
    Effect,
    Event,
    Location,
    Talent,
    Action,

    NotImplemented // used to return default values from placeholder routines
};

class EngineType;
class ScriptObject;

struct Variable {
    VariableType type { VariableType::Void };
    std::string strValue;
    glm::vec3 vecValue { 0.0f };
    std::shared_ptr<EngineType> engineType;
    std::shared_ptr<ExecutionContext> context;

    union {
        int32_t intValue { 0 };
        uint32_t objectId;
        float floatValue;
    };

    Variable() = default;

    static Variable ofInt(int value);
    static Variable ofFloat(float value);
    static Variable ofString(std::string value);
    static Variable ofVector(glm::vec3 value);
    static Variable ofObject(uint32_t objectId);
    static Variable ofEffect(std::shared_ptr<EngineType> engineType);
    static Variable ofEvent(std::shared_ptr<EngineType> engineType);
    static Variable ofLocation(std::shared_ptr<EngineType> engineType);
    static Variable ofTalent(std::shared_ptr<EngineType> engineType);
    static Variable ofAction(std::shared_ptr<ExecutionContext> context);
    static Variable notImplemented();

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
