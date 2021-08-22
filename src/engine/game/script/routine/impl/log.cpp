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

#include "declarations.h"

#include "../../../../common/log.h"
#include "../../../../script/exception/notimpl.h"

#include "../../../object/object.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable printString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    string str(getStringOrElse(args, 0));
    info(str);
    return Variable::ofNull();
}

Variable printFloat(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    // TODO: use formatting parameters
    float value = getFloatOrElse(args, 0);
    int width = getIntOrElse(args, 1, 18);
    int decimals = getIntOrElse(args, 2, 9);

    info(to_string(value));

    return Variable::ofNull();
}

Variable printInteger(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    int value = getIntOrElse(args, 0);
    info(to_string(value));
    return Variable::ofNull();
}

Variable printObject(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObject(game, args, 0, ctx);
    info(to_string(object->id()));
    return Variable::ofNull();
}

Variable printVector(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    glm::vec3 value(getVectorOrElse(args, 0));
    info(boost::format("%f %f %f") % value.x % value.y % value.z);
    return Variable::ofNull();
}

Variable writeTimestampedLogEntry(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable aurPostString(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
