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
        debug("Script: getFacingFromLocation: location is invalid");
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
        float facing = object->facing();
        result.engineType = make_shared<Location>(move(position), facing);
    } else {
        debug("Script: getLocation: object is invalid");
    }

    return move(result);
}

Variable Routines::getPositionFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto location = getLocationEngineType(args, 0);
    if (!location) {
        debug("Script: getPositionFromLocation: location is invalid");
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

Variable Routines::getDistanceBetweenLocations(const VariablesList &args, ExecutionContext &ctx) {
    auto locationA = getLocationEngineType(args, 0);
    if (!locationA) {
        debug("Script: getDistanceBetweenLocations: locationA is invalid");
        return 0.0f;
    }
    auto locationB = getLocationEngineType(args, 1);
    if (!locationB) {
        debug("Script: getDistanceBetweenLocations: locationB is invalid");
        return 0.0f;
    }
    return glm::distance(locationA->position(), locationB->position());
}

Variable Routines::getDistanceBetweenLocations2D(const VariablesList &args, ExecutionContext &ctx) {
    auto locationA = getLocationEngineType(args, 0);
    if (!locationA) {
        debug("Script: getDistanceBetweenLocations2D: locationA is invalid");
        return 0.0f;
    }
    auto locationB = getLocationEngineType(args, 1);
    if (!locationB) {
        debug("Script: getDistanceBetweenLocations2D: locationB is invalid");
        return 0.0f;
    }
    return glm::distance(glm::vec2(locationA->position()), glm::vec2(locationB->position()));
}

Variable Routines::getStartingLocation(const VariablesList &args, ExecutionContext &ctx) {
    const ModuleInfo &info = _game->module()->info();
    auto location = make_shared<Location>(info.entryPosition, info.entryFacing);
    return Variable(VariableType::Location, move(location));
}

} // namespace game

} // namespace reone
