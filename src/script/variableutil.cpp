/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "variableutil.h"

#include "../common/exception/argument.h"

using namespace std;

namespace reone {

namespace script {

string describeVariableType(VariableType type) {
    switch (type) {
    case VariableType::Void:
        return "void";
    case VariableType::Int:
        return "int";
    case VariableType::Float:
        return "float";
    case VariableType::String:
        return "string";
    case VariableType::Vector:
        return "vector";
    case VariableType::Object:
        return "object";
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
        throw ArgumentException("Cannot describe variable type: " + to_string(static_cast<int>(type)));
    }
}

} // namespace script

} // namespace reone
