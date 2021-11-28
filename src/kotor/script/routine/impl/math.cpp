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

#include "../declarations.h"

#include "../../../../game/script/routine/argutil.h"
#include "../../../../game/script/routine/objectutil.h"

using namespace std;

using namespace reone::game;
using namespace reone::script;

namespace reone {

namespace kotor {

namespace routine {

Variable fabs(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);
    return Variable::ofFloat(glm::abs(value));
}

Variable cos(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);
    float result = glm::radians(value);

    return Variable::ofFloat(glm::cos(value));
}

Variable sin(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);
    float result = glm::radians(value);

    return Variable::ofFloat(glm::sin(value));
}

Variable tan(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);
    float result = glm::radians(value);

    return Variable::ofFloat(glm::tan(value));
}

Variable acos(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);

    float result = 0.0f;
    if (glm::abs(value) <= 1.0f) {
        result = glm::degrees(glm::acos(value));
    }

    return Variable::ofFloat(result);
}

Variable asin(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);

    float result = 0.0f;
    if (glm::abs(value) <= 1.0f) {
        result = glm::degrees(glm::asin(value));
    }

    return Variable::ofFloat(result);
}

Variable atan(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);
    float result = glm::degrees(glm::atan(value));

    return Variable::ofFloat(result);
}

Variable log(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);

    float result = 0.0f;
    if (value > 0.0f) {
        result = glm::log(value);
    }

    return Variable::ofFloat(result);
}

Variable pow(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);
    float exponent = getFloat(args, 1);

    float result = 0.0f;
    if (value != 0.0f || exponent >= 0.0f) {
        result = glm::pow(value, exponent);
    }

    return Variable::ofFloat(result);
}

Variable sqrt(const vector<Variable> &args, const RoutineContext &ctx) {
    float value = getFloat(args, 0);

    float result = 0.0f;
    if (value >= 0.0f) {
        result = glm::sqrt(value);
    }

    return Variable::ofFloat(result);
}

Variable abs(const vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);
    int result = glm::abs(value);

    return Variable::ofInt(result);
}

} // namespace routine

} // namespace kotor

} // namespace reone
