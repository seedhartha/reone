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

#include "variable.h"

#include <stdexcept>

#include <boost/format.hpp>

#include "enginetype.h"
#include "object.h"

using namespace std;

namespace reone {

namespace script {

Variable Variable::operator+(const Variable &other) const {
    if (type == VariableType::Int && other.type == VariableType::Int) {
        return intValue + other.intValue;
    }
    if (type == VariableType::Int && other.type == VariableType::Float) {
        return intValue + other.floatValue;
    }
    if (type == VariableType::Float && other.type == VariableType::Int) {
        return floatValue + other.intValue;
    }
    if (type == VariableType::Float && other.type == VariableType::Float) {
        return floatValue + other.floatValue;
    }
    if (type == VariableType::String && other.type == VariableType::String) {
        return strValue + other.strValue;
    }

    throw logic_error(str(boost::format("Unsupported variable types: %02x %02x") % static_cast<int>(type) % static_cast<int>(other.type)));
}

Variable Variable::operator-(const Variable &other) const {
    if (type == VariableType::Int && other.type == VariableType::Int) {
        return intValue - other.intValue;
    }
    if (type == VariableType::Int && other.type == VariableType::Float) {
        return intValue - other.floatValue;
    }
    if (type == VariableType::Float && other.type == VariableType::Int) {
        return floatValue - other.intValue;
    }
    if (type == VariableType::Float && other.type == VariableType::Float) {
        return floatValue - other.floatValue;
    }

    throw logic_error(str(boost::format("Unsupported variable types: %02x %02x") % static_cast<int>(type) % static_cast<int>(other.type)));
}

Variable Variable::operator*(const Variable &other) const {
    if (type == VariableType::Int && other.type == VariableType::Int) {
        return intValue * other.intValue;
    }
    if (type == VariableType::Int && other.type == VariableType::Float) {
        return intValue * other.floatValue;
    }
    if (type == VariableType::Float && other.type == VariableType::Int) {
        return floatValue * other.intValue;
    }
    if (type == VariableType::Float && other.type == VariableType::Float) {
        return floatValue * other.floatValue;
    }

    throw logic_error(str(boost::format("Unsupported variable types: %02x %02x") % static_cast<int>(type) % static_cast<int>(other.type)));
}

Variable Variable::operator/(const Variable &other) const {
    if (type == VariableType::Int && other.type == VariableType::Int) {
        return intValue / other.intValue;
    }
    if (type == VariableType::Int && other.type == VariableType::Float) {
        return intValue / other.floatValue;
    }
    if (type == VariableType::Float && other.type == VariableType::Int) {
        return floatValue / other.intValue;
    }
    if (type == VariableType::Float && other.type == VariableType::Float) {
        return floatValue / other.floatValue;
    }

    throw logic_error(str(boost::format("Unsupported variable types: %02x %02x") % static_cast<int>(type) % static_cast<int>(other.type)));
}

Variable::Variable(int value) : type(VariableType::Int), intValue(value) {
}

Variable::Variable(float value) : type(VariableType::Float), floatValue(value) {
}

Variable::Variable(string value) : type(VariableType::String), strValue(move(value)) {
}

Variable::Variable(glm::vec3 value) : type(VariableType::Vector), vecValue(move(value)) {
}

Variable::Variable(const shared_ptr<ScriptObject> &object) : type(VariableType::Object), object(object) {
}

Variable::Variable(VariableType type, const shared_ptr<EngineType> &engineType) : type(type), engineType(engineType) {
}

Variable::Variable(const ExecutionContext &ctx) : type(VariableType::Action), context(ctx) {
}

bool Variable::operator==(const Variable &other) const {
    if (type != other.type) return false;

    switch (type) {
        case VariableType::Int:
            return intValue == other.intValue;
        case VariableType::Float:
            return floatValue == other.floatValue;
        case VariableType::String:
            return strValue == other.strValue;
        case VariableType::Object:
            return object == other.object;
        case VariableType::Effect:
        case VariableType::Event:
        case VariableType::Location:
        case VariableType::Talent:
            return engineType == other.engineType;
        default:
            throw logic_error("Unsupported variable type: " + to_string(static_cast<int>(type)));
    }

    return true;
}

bool Variable::operator!=(const Variable &other) const {
    return !operator==(other);
}

bool Variable::operator<(const Variable &other) const {
    switch (type) {
        case VariableType::Int:
            return intValue < other.intValue;
        case VariableType::Float:
            return floatValue < other.floatValue;
        default:
            throw logic_error(str(boost::format("Unsupported variable type: %02x") % static_cast<int>(type)));
    }
}

bool Variable::operator<=(const Variable &other) const {
    switch (type) {
        case VariableType::Int:
            return intValue <= other.intValue;
        case VariableType::Float:
            return floatValue <= other.floatValue;
        default:
            throw logic_error(str(boost::format("Unsupported variable type: %02x") % static_cast<int>(type)));
    }
}

bool Variable::operator>(const Variable &other) const {
    switch (type) {
        case VariableType::Int:
            return intValue > other.intValue;
        case VariableType::Float:
            return floatValue > other.floatValue;
        default:
            throw logic_error(str(boost::format("Unsupported variable type: %02x") % static_cast<int>(type)));
    }
}

bool Variable::operator>=(const Variable &other) const {
    switch (type) {
        case VariableType::Int:
            return intValue >= other.intValue;
        case VariableType::Float:
            return floatValue >= other.floatValue;
        default:
            throw logic_error(str(boost::format("Unsupported variable type: %02x") % static_cast<int>(type)));
    }
}

const string Variable::toString() const {
    static string empty;

    switch (type) {
        case VariableType::Void:
            return "void";
        case VariableType::Int:
            return to_string(intValue);
        case VariableType::Float:
            return to_string(floatValue);
        case VariableType::Object:
            return object ? to_string(object->id()) : empty;
        case VariableType::String:
            return str(boost::format("\"%s\"") % strValue);
        default:
            return "[not implemented]";
    }
}

} // namespace script

} // namespace reone
