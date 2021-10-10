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
 *  Implementation of time-related routines.
 */

#include "../declarations.h"

#include "../../../../../common/logutil.h"
#include "../../../../../script/exception/notimpl.h"

#include "../../../object/object.h"
#include "../../../script/routine/argutil.h"
#include "../../../script/routine/context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable printString(const vector<Variable> &args, const RoutineContext &ctx) {
    string str(getString(args, 0));

    info(str);

    return Variable::ofNull();
}

Variable printFloat(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: use formatting parameters
    float value = getFloat(args, 0);
    int width = getIntOrElse(args, 1, 18);
    int decimals = getIntOrElse(args, 2, 9);

    info(to_string(value));

    return Variable::ofNull();
}

Variable printInteger(const vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);

    info(to_string(value));

    return Variable::ofNull();
}

Variable printObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto object = getObject(args, 0, ctx);

    info(to_string(object->id()));

    return Variable::ofNull();
}

Variable printVector(const vector<Variable> &args, const RoutineContext &ctx) {
    glm::vec3 value(getVector(args, 0));

    info(boost::format("%f %f %f") % value.x % value.y % value.z);

    return Variable::ofNull();
}

Variable writeTimestampedLogEntry(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable aurPostString(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
