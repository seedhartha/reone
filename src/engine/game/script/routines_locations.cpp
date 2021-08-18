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
 *  Implementation of routines related to the Location engine type.
 */

#include "routines.h"

#include "../../common/log.h"

#include "../game.h"
#include "../location.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getFacingFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    float result = -1.0f;

    auto location = getLocationEngineType(args, 0);
    if (location) {
        result = glm::degrees(location->facing());
    } else {
        debug("Script: getFacingFromLocation: location is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getLocation(const VariablesList &args, ExecutionContext &ctx) {
    shared_ptr<Location> result;

    auto object = getSpatialObject(args, 0, ctx);
    if (object) {
        glm::vec3 position(object->position());
        float facing = object->getFacing();
        result = make_shared<Location>(move(position), facing);
    } else {
        debug("Script: getLocation: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofLocation(move(result));
}

Variable Routines::getPositionFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 result(0.0f);

    auto location = getLocationEngineType(args, 0);
    if (location) {
        result = location->position();
    } else {
        debug("Script: getPositionFromLocation: location is invalid", 1, DebugChannels::script);
    }

    return Variable::ofVector(move(result));
}

Variable Routines::location(const VariablesList &args, ExecutionContext &ctx) {
    glm::vec3 position(getVector(args, 0));
    float orientation = glm::radians(getFloat(args, 1));
    auto location = make_shared<Location>(move(position), orientation);

    return Variable::ofLocation(location);
}

Variable Routines::getDistanceBetweenLocations(const VariablesList &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto locationA = getLocationEngineType(args, 0);
    auto locationB = getLocationEngineType(args, 1);

    if (locationA && locationB) {
        result = glm::distance(locationA->position(), locationB->position());
    } else if (!locationA) {
        debug("Script: getDistanceBetweenLocations: locationA is invalid", 1, DebugChannels::script);
    } else if (!locationB) {
        debug("Script: getDistanceBetweenLocations: locationB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getDistanceBetweenLocations2D(const VariablesList &args, ExecutionContext &ctx) {
    float result = 0.0f;
    auto locationA = getLocationEngineType(args, 0);
    auto locationB = getLocationEngineType(args, 1);

    if (locationA && locationB) {
        result = glm::distance(glm::vec2(locationA->position()), glm::vec2(locationB->position()));
    } else if (!locationA) {
        debug("Script: getDistanceBetweenLocations2D: locationA is invalid", 1, DebugChannels::script);
    } else if (!locationB) {
        debug("Script: getDistanceBetweenLocations2D: locationB is invalid", 1, DebugChannels::script);
    }

    return Variable::ofFloat(result);
}

Variable Routines::getStartingLocation(const VariablesList &args, ExecutionContext &ctx) {
    const ModuleInfo &info = _game.module()->info();
    auto location = make_shared<Location>(info.entryPosition, info.entryFacing);
    return Variable::ofLocation(move(location));
}

} // namespace game

} // namespace reone
