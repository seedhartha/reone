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

#include "reone/script/variable.h"

using namespace std;

namespace reone {

namespace script {

Variable Variable::ofNull() {
    Variable result;
    result.type = VariableType::Void;
    return move(result);
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

Variable Variable::ofObject(uint32_t objectId) {
    Variable result;
    result.type = VariableType::Object;
    result.objectId = objectId;
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

const string Variable::toString() const {
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
        return to_string(objectId);
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
