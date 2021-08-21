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

#include "../../routines.h"

#include "../../../../common/log.h"

#include "../../../object/object.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::printString(const VariablesList &args, ExecutionContext &ctx) {
    string str(getString(args, 0));
    info(str);
    return Variable();
}

Variable Routines::printFloat(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: use formatting parameters
    float value = getFloat(args, 0);
    int width = getInt(args, 1, 18);
    int decimals = getInt(args, 2, 9);

    info(to_string(value));

    return Variable();
}

Variable Routines::printInteger(const VariablesList &args, ExecutionContext &ctx) {
    int value = getInt(args, 0);
    info(to_string(value));
    return Variable();
}

Variable Routines::printObject(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    info(to_string(object->id()));
    return Variable();
}

Variable Routines::printVector(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 value(getVector(args, 0));
    info(boost::format("%f %f %f") % value.x % value.y % value.z);
    return Variable();
}

Variable Routines::writeTimestampedLogEntry(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::aurPostString(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

} // namespace game

} // namespace reone
