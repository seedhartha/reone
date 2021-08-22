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

/** @file
 *  Implementation of math-related routines.
 */

#include "declarations.h"

#include "../../../../script/types.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable fabs(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float value = getFloatOrElse(args, 0);
    return Variable::ofFloat(glm::abs(value));
}

Variable cos(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float value = glm::radians(getFloatOrElse(args, 0));
    return Variable::ofFloat(glm::cos(value));
}

Variable sin(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float value = glm::radians(getFloatOrElse(args, 0));
    return Variable::ofFloat(glm::sin(value));
}

Variable tan(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float value = glm::radians(getFloatOrElse(args, 0));
    return Variable::ofFloat(glm::tan(value));
}

Variable acos(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = 0.0f;
    float value = getFloatOrElse(args, 0);
    if (glm::abs(value) <= 1.0f) {
        result = glm::degrees(glm::acos(value));
    }
    return Variable::ofFloat(result);
}

Variable asin(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = 0.0f;
    float value = getFloatOrElse(args, 0);
    if (glm::abs(value) <= 1.0f) {
        result = glm::degrees(glm::asin(value));
    }
    return Variable::ofFloat(result);
}

Variable atan(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float value = getFloatOrElse(args, 0);
    return Variable::ofFloat(glm::degrees(glm::atan(value)));
}

Variable log(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = 0.0f;
    float value = getFloatOrElse(args, 0);
    if (value > 0.0f) {
        result = glm::log(value);
    }
    return Variable::ofFloat(result);
}

Variable pow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = 0.0f;
    float value = getFloatOrElse(args, 0);
    float exponent = getFloatOrElse(args, 1);
    if (value != 0.0f || exponent >= 0.0f) {
        result = glm::pow(value, exponent);
    }
    return Variable::ofFloat(result);
}

Variable sqrt(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    float result = 0.0f;
    float value = getFloatOrElse(args, 0);
    if (value >= 0.0f) {
        result = glm::sqrt(value);
    }
    return Variable::ofFloat(result);
}

Variable abs(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int value = getIntOrElse(args, 0);
    return Variable::ofInt(glm::abs(value));
}

} // namespace routine

} // namespace game

} // namespace reone
