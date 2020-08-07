/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <cassert>
#include <stdexcept>

#include <boost/format.hpp>

using namespace std;

using namespace glm;

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

Variable::Variable(VariableType type) : type(type) {
}

Variable::Variable(int value) : type(VariableType::Int), intValue(value) {
}

Variable::Variable(float value) : type(VariableType::Float), floatValue(value) {
}

Variable::Variable(const string &value) : type(VariableType::String), strValue(value) {
}

Variable::Variable(const vec3 &value) : type(VariableType::Vector), vecValue(value) {
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
            return objectId == other.objectId;
        case VariableType::Effect:
        case VariableType::Event:
        case VariableType::Location:
        case VariableType::Talent:
            return engineTypeId == other.engineTypeId;
        default:
            throw logic_error("Unsupported variable type: " + to_string(static_cast<int>(type)));
    }

    return true;
}

bool Variable::operator!=(const Variable &other) const {
    return !operator==(other);
}

bool Variable::operator<(const Variable &other) const {
    assert(type == other.type);
    assert(type == VariableType::Int || type == VariableType::Float);

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
    assert(type == other.type);
    assert(type == VariableType::Int || type == VariableType::Float);

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
    assert(type == other.type);
    assert(type == VariableType::Int || type == VariableType::Float);

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
    assert(type == other.type);

    switch (type) {
        case VariableType::Int:
            return intValue >= other.intValue;
        case VariableType::Float:
            return floatValue >= other.floatValue;
        default:
            throw logic_error(str(boost::format("Unsupported variable type: %02x") % static_cast<int>(type)));
    }
}

} // namespace script

} // namespace reone
