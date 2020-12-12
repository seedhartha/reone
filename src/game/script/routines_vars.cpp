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

#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getGlobalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return _game->getGlobalBoolean(id);
}

Variable Routines::getGlobalNumber(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return _game->getGlobalNumber(id);
}

Variable Routines::getGlobalString(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    return _game->getGlobalString(id);
}

Variable Routines::getLocalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    if (!object) {
        warn("Routines: getLocalBoolean: object is invalid");
        return false;
    }
    int index = getInt(args, 1);

    return _game->getLocalBoolean(object->id(), index);
}

Variable Routines::getLocalNumber(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    if (!object) {
        warn("Routines: getLocalNumber: object is invalid");
        return false;
    }
    int index = getInt(args, 1);

    return _game->getLocalNumber(object->id(), index);
}

Variable Routines::setGlobalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    bool value = getBool(args, 1);

    _game->setGlobalBoolean(id, value);

    return Variable();
}

Variable Routines::setGlobalNumber(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    int value = getInt(args, 1);

    _game->setGlobalNumber(id, value);

    return Variable();
}

Variable Routines::setGlobalString(const VariablesList &args, ExecutionContext &ctx) {
    string id(getString(args, 0));
    string value(getString(args, 1));

    _game->setGlobalString(id, value);

    return Variable();
}

Variable Routines::setLocalBoolean(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    if (!object) {
        warn("Routines: setLocalBoolean: object is invalid");
        return false;
    }
    int index = getInt(args, 1);
    bool value = getBool(args, 2);
    _game->setLocalBoolean(object->id(), index, value);

    return Variable();
}

Variable Routines::setLocalNumber(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    if (!object) {
        warn("Routines: setLocalNumber: object is invalid");
        return false;
    }
    int index = getInt(args, 1);
    int value = getInt(args, 2);
    _game->setLocalNumber(object->id(), index, value);

    return Variable();
}

} // namespace game

} // namespace reone
