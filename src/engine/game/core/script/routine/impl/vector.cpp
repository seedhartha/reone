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

#include "../declarations.h"

#include "../../../../../script/exception/notimpl.h"
#include "../../../../../script/types.h"

#include "../../../script/routine/argutil.h"
#include "../../../script/routine/context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable vectorMagnitude(const vector<Variable> &args, const RoutineContext &ctx) {
    glm::vec3 value(getVector(args, 0));
    return Variable::ofFloat(glm::length(value));
}

Variable vectorNormalize(const vector<Variable> &args, const RoutineContext &ctx) {
    glm::vec3 value(getVector(args, 0));
    return Variable::ofVector(glm::normalize(value));
}

Variable vectorCreate(const vector<Variable> &args, const RoutineContext &ctx) {
    float x = getFloat(args, 0);
    float y = getFloat(args, 1);
    float z = getFloat(args, 2);
    return Variable::ofVector(glm::vec3(x, y, z));
}

Variable angleToVector(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable vectorToAngle(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
