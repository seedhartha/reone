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

#include "routines.h"

#include "glm/glm.hpp"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::fabs(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    return Variable::ofFloat(glm::abs(value));
}

Variable Routines::cos(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    return Variable::ofFloat(glm::cos(value));
}

Variable Routines::sin(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    return Variable::ofFloat(glm::sin(value));
}

Variable Routines::tan(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    return Variable::ofFloat(glm::tan(value));
}

Variable Routines::acos(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    if (value > 1 || value < -1) return Variable::ofFloat(0.0f);

    return Variable::ofFloat(glm::acos(value));
}

Variable Routines::asin(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    if (value > 1 || value < -1) return Variable::ofFloat(0.0f);

    return Variable::ofFloat(glm::asin(value));
}

Variable Routines::atan(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    return Variable::ofFloat(glm::atan(value));
}

Variable Routines::log(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    if (value <= 0.0f) return Variable::ofFloat(0.0f);

    return Variable::ofFloat(glm::log(value));
}

Variable Routines::pow(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    float exponent = getFloat(args, 1);
    if (value == 0.0f && exponent < 0.0f) return Variable::ofFloat(0.0f);

    return Variable::ofFloat(glm::pow(value, exponent));
}

Variable Routines::sqrt(const VariablesList &args, ExecutionContext &ctx) {
    float value = getFloat(args, 0);
    if (value < 0.0f) return Variable::ofFloat(0.0f);

    return Variable::ofFloat(glm::sqrt(value));
}

Variable Routines::abs(const VariablesList &args, ExecutionContext &ctx) {
    int value = getInt(args, 0);
    return Variable::ofInt(glm::abs(value));
}

Variable Routines::vectorCreate(const VariablesList &args, ExecutionContext &ctx) {
    float x = getFloat(args, 0);
    float y = getFloat(args, 1);
    float z = getFloat(args, 2);
    return Variable::ofVector(glm::vec3(x, y, z));
}

Variable Routines::vectorNormalize(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofVector(glm::normalize(getVector(args, 0)));
}

Variable Routines::vectorMagnitude(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(getVector(args, 0).length());
}

} // namespace game

} // namespace reone
