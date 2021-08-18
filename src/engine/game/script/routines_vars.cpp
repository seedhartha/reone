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
 *  Implementation of routines related to global and local variables.
 */

#include "routines.h"

#include "../../common/log.h"

#include "../game.h"
#include "../location.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getGlobalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofInt(static_cast<int>(_game.getGlobalBoolean(id)));
}

Variable Routines::setGlobalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    bool value = getBool(args, 1);

    _game.setGlobalBoolean(id, value);

    return Variable();
}

Variable Routines::getGlobalNumber(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofInt(_game.getGlobalNumber(id));
}

Variable Routines::setGlobalNumber(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    int value = getInt(args, 1);

    _game.setGlobalNumber(id, value);

    return Variable();
}

Variable Routines::getGlobalString(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofString(_game.getGlobalString(id));
}

Variable Routines::setGlobalString(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    string value(getString(args, 1));

    _game.setGlobalString(id, value);

    return Variable();
}

Variable Routines::getLocalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);

    if (object) {
        result = object->getLocalBoolean(index);
    } else {
        debug("Script: getLocalBoolean: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setLocalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    bool value = getBool(args, 2);

    if (object) {
        object->setLocalBoolean(index, value);
    } else {
        debug("Script: setLocalBoolean: object is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getLocalNumber(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);

    if (object) {
        result = object->getLocalNumber(index);
    } else {
        debug("Script: getLocalNumber: object is invalid", 1, DebugChannels::script);
    }

    return Variable::ofInt(result);
}

Variable Routines::setLocalNumber(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0, ctx);
    int index = getInt(args, 1);
    int value = getInt(args, 2);

    if (object) {
        object->setLocalNumber(index, value);
    } else {
        debug("Script: setLocalNumber: object is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::getGlobalLocation(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return Variable::ofLocation(_game.getGlobalLocation(id));
}

Variable Routines::setGlobalLocation(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    auto value = getLocationEngineType(args, 1);

    if (value) {
        _game.setGlobalLocation(id, value);
    } else {
        debug("Script: setGlobalLocation: value is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

} // namespace game

} // namespace reone
