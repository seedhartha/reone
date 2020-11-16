/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

Variable Routines::fabs(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    return glm::abs(value);
}

Variable Routines::cos(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    return glm::cos(value);
}

Variable Routines::sin(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    return glm::sin(value);
}

Variable Routines::tan(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    return glm::tan(value);
}

Variable Routines::acos(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    if (value > 1 || value < -1) return 0.0f;

    return glm::acos(value);
}

Variable Routines::asin(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    if (value > 1 || value < -1) return 0.0f;

    return glm::asin(value);
}

Variable Routines::atan(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    return glm::atan(value);
}

Variable Routines::log(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    if (value <= 0.0f) return 0.0f;

    return glm::log(value);
}

Variable Routines::pow(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    float exponent = args[1].floatValue;
    if (value == 0.0f && exponent < 0.0f) return 0.0f;

    return glm::pow(value, exponent);
}

Variable Routines::sqrt(const vector<Variable> &args, ExecutionContext &ctx) {
    float value = args[0].floatValue;
    if (value < 0.0f) return 0.0f;

    return glm::sqrt(value);
}

Variable Routines::abs(const vector<Variable> &args, ExecutionContext &ctx) {
    int value = args[0].intValue;
    return glm::abs(value);
}

Variable Routines::vectorCreate(const vector<Variable> &args, ExecutionContext &ctx) {
    float x = args[0].floatValue;
    float y = args[1].floatValue;
    float z = args[2].floatValue;
    return glm::vec3(x, y, z);
}

Variable Routines::vectorNormalize(const vector<Variable> &args, ExecutionContext &ctx) {
    return glm::normalize(args[0].vecValue);
}

Variable Routines::vectorMagnitude(const vector<Variable> &args, ExecutionContext &ctx) {
    return args[0].vecValue.length();
}

} // namespace game

} // namespace reone
