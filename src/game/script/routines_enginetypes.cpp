/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../common/log.h"

#include "../enginetype/location.h"
#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getFacingFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto location = getLocationEngineType(args, 0);
    if (!location) {
        warn("Routines: getFacingFromLocation: location is invalid");
        return -1.0f;
    }
    return location->facing();
}

Variable Routines::getLocation(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Location;

    auto object = getSpatialObject(args, 0);
    if (object) {
        glm::vec3 position(object->position());
        float facing = object->heading();
        result.engineType = make_shared<Location>(move(position), facing);
    } else {
        warn("Routines: getLocation: object is invalid");
    }

    return move(result);
}

Variable Routines::getPositionFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto location = getLocationEngineType(args, 0);
    if (!location) {
        warn("Routines: getPositionFromLocation: location is invalid");
        return glm::vec3(0.0f);
    }
    return location->position();
}

Variable Routines::location(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = getFloat(args, 1);
    auto location = make_shared<Location>(move(position), orientation);

    return Variable(VariableType::Location, location);
}

} // namespace game

} // namespace reone
