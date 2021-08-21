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
 *  Implementation of vector-related routines.
 */

#include "../../routines.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::vectorMagnitude(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofFloat(glm::length(getVector(args, 0)));
}

Variable Routines::vectorNormalize(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofVector(glm::normalize(getVector(args, 0)));
}

Variable Routines::vectorCreate(const VariablesList &args, ExecutionContext &ctx) {
    float x = getFloat(args, 0);
    float y = getFloat(args, 1);
    float z = getFloat(args, 2);
    return Variable::ofVector(glm::vec3(x, y, z));
}

Variable Routines::angleToVector(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::vectorToAngle(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

} // namespace game

} // namespace reone
