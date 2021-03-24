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
        return Variable::ofInt(intValue + other.intValue);
    }
    if (type == VariableType::Int && other.type == VariableType::Float) {
        return Variable::ofFloat(intValue + other.floatValue);
    }
    if (type == VariableType::Float && other.type == VariableType::Int) {
        return Variable::ofFloat(floatValue + other.intValue);
    }
    if (type == VariableType::Float && other.type == VariableType::Float) {
        return Variable::ofFloat(floatValue + other.floatValue);
    }
    if (type == VariableType::String && other.type == VariableType::String) {
        return Variable::ofString(strValue + other.strValue);
    }

    throw logic_error(str(boost::format("Unsupported variable types: %02x %02x") % static_cast<int>(type) % static_cast<int>(other.type)));
}

Variable Variable::operator-(const Variable &other) const {
    if (type == VariableType::Int && other.type == VariableType::Int) {
        return Variable::ofInt(intValue - other.intValue);
    }
    if (type == VariableType::Int && other.type == VariableType::Float) {
        return Variable::ofFloat(intValue - other.floatValue);
    }
    if (type == VariableType::Float && other.type == VariableType::Int) {
        return Variable::ofFloat(floatValue - other.intValue);
    }
    if (type == VariableType::Float && other.type == VariableType::Float) {
        return Variable::ofFloat(floatValue - other.floatValue);
    }

    throw logic_error(str(boost::format("Unsupported variable types: %02x %02x") % static_cast<int>(type) % static_cast<int>(other.type)));
}

Variable Variable::operator*(const Variable &other) const {
    if (type == VariableType::Int && other.type == VariableType::Int) {
        return Variable::ofInt(intValue * other.intValue);
    }
    if (type == VariableType::Int && other.type == VariableType::Float) {
        return Variable::ofFloat(intValue * other.floatValue);
    }
    if (type == VariableType::Float && other.type == VariableType::Int) {
        return Variable::ofFloat(floatValue * other.intValue);
    }
    if (type == VariableType::Float && other.type == VariableType::Float) {
        return Variable::ofFloat(floatValue * other.floatValue);
    }

    throw logic_error(str(boost::format("Unsupported variable types: %02x %02x") % static_cast<int>(type) % static_cast<int>(other.type)));
}

Variable Variable::operator/(const Variable &other) const {
    if (type == VariableType::Int && other.type == VariableType::Int) {
        return Variable::ofInt(intValue / other.intValue);
    }
    if (type == VariableType::Int && other.type == VariableType::Float) {
        return Variable::ofFloat(intValue / other.floatValue);
    }
    if (type == VariableType::Float && other.type == VariableType::Int) {
        return Variable::ofFloat(floatValue / other.intValue);
    }
    if (type == VariableType::Float && other.type == VariableType::Float) {
        return Variable::ofFloat(floatValue / other.floatValue);
    }

    throw logic_error(str(boost::format("Unsupported variable types: %02x %02x") % static_cast<int>(type) % static_cast<int>(other.type)));
}

Variable Variable::ofInt(int value) {
    Variable result;
    result.type = VariableType::Int;
    result.intValue = value;
    return move(result);
}

Variable Variable::ofFloat(float value) {
    Variable result;
    result.type = VariableType::Float;
    result.floatValue = value;
    return move(result);
}

Variable Variable::ofString(string value) {
    Variable result;
    result.type = VariableType::String;
    result.strValue = move(value);
    return move(result);
}

Variable Variable::ofVector(glm::vec3 value) {
    Variable result;
    result.type = VariableType::Vector;
    result.vecValue = move(value);
    return move(result);
}

Variable Variable::ofObject(shared_ptr<ScriptObject> object) {
    Variable result;
    result.type = VariableType::Object;
    result.object = move(object);
    return move(result);
}

Variable Variable::ofEffect(shared_ptr<EngineType> engineType) {
    Variable result;
    result.type = VariableType::Effect;
    result.engineType = move(engineType);
    return move(result);
}

Variable Variable::ofEvent(shared_ptr<EngineType> engineType) {
    Variable result;
    result.type = VariableType::Event;
    result.engineType = move(engineType);
    return move(result);
}

Variable Variable::ofLocation(shared_ptr<EngineType> engineType) {
    Variable result;
    result.type = VariableType::Location;
    result.engineType = move(engineType);
    return move(result);
}

Variable Variable::ofTalent(shared_ptr<EngineType> engineType) {
    Variable result;
    result.type = VariableType::Talent;
    result.engineType = move(engineType);
    return move(result);
}

Variable Variable::ofAction(shared_ptr<ExecutionContext> context) {
    Variable result;
    result.type = VariableType::Action;
    result.context = move(context);
    return move(result);
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
        case VariableType::String:
            return str(boost::format("\"%s\"") % strValue);
        case VariableType::Object:
            return to_string(object ? object->id() : kObjectInvalid);
        case VariableType::Vector:
            return str(boost::format("[%f,%f,%f]") % vecValue.x % vecValue.y % vecValue.z);
        case VariableType::Effect:
            return "effect";
        case VariableType::Event:
            return "event";
        case VariableType::Location:
            return "location";
        case VariableType::Talent:
            return "talent";
        case VariableType::Action:
            return "action";
        default:
            throw logic_error("Unsupported variable type: " + to_string(static_cast<int>(type)));
    }
}

} // namespace script

} // namespace reone
