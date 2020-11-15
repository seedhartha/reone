/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getGlobalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getGlobalBoolean(args[0].strValue);
}

Variable Routines::getGlobalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getGlobalNumber(args[0].strValue);
}

Variable Routines::getLocalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    return object ? _game->getLocalBoolean(object->id(), args[1].intValue) : false;
}

Variable Routines::getLocalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    return object ? _game->getLocalNumber(object->id(), args[1].intValue) : false;
}

Variable Routines::setGlobalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    _game->setGlobalBoolean(args[0].strValue, args[1].intValue);
    return Variable();
}

Variable Routines::setGlobalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    _game->setGlobalNumber(args[0].strValue, args[1].intValue);
    return Variable();
}

Variable Routines::setLocalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        _game->setLocalBoolean(object->id(), args[1].intValue, args[2].intValue);
    }
    return Variable();
}

Variable Routines::setLocalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        _game->setLocalNumber(object->id(), args[1].intValue, args[2].intValue);
    }
    return Variable();
}

} // namespace game

} // namespace reone
