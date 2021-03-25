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

#include "../enginetype/effect.h"
#include "../enginetype/event.h"
#include "../enginetype/location.h"
#include "../enginetype/talent.h"
#include "../object/creature.h"
#include "../object/door.h"
#include "../object/item.h"
#include "../object/sound.h"

#include "../game.h"
#include "../gameidutil.h"

using namespace std;

using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

Routines &Routines::instance() {
    static Routines instance;
    return instance;
}

void Routines::init(Game *game) {
    _game = game;

    if (isTSL(game->gameId())) {
        addTslRoutines();
    } else {
        addKotorRoutines();
    }
}

Routines::~Routines() {
    deinit();
}

void Routines::deinit() {
    _routines.clear();
}

void Routines::add(const string &name, VariableType retType, const VariableTypesList &argTypes) {
    _routines.emplace_back(name, retType, argTypes);
}

const Routine &Routines::get(int index) {
    return _routines[index];
}

bool Routines::getBool(const VariablesList &args, int index, bool defValue) const {
    return isOutOfRange(args, index) ?
        defValue :
        static_cast<bool>(args[index].intValue);
}

int Routines::getInt(const VariablesList &args, int index, int defValue) const {
    return isOutOfRange(args, index) ? defValue : args[index].intValue;
}

float Routines::getFloat(const VariablesList &args, int index, float defValue) const {
    return isOutOfRange(args, index) ? defValue : args[index].floatValue;
}

string Routines::getString(const VariablesList &args, int index, string defValue) const {
    return isOutOfRange(args, index) ? move(defValue) : args[index].strValue;
}

glm::vec3 Routines::getVector(const VariablesList &args, int index, glm::vec3 defValue) const {
    return isOutOfRange(args, index) ? move(defValue) : args[index].vecValue;
}

shared_ptr<Object> Routines::getCaller(ExecutionContext &ctx) const {
    return static_pointer_cast<Object>(ctx.caller);
}

shared_ptr<SpatialObject> Routines::getCallerAsSpatial(ExecutionContext &ctx) const {
    return dynamic_pointer_cast<SpatialObject>(ctx.caller);
}

shared_ptr<Creature> Routines::getCallerAsCreature(ExecutionContext &ctx) const {
    return dynamic_pointer_cast<Creature>(ctx.caller);
}

shared_ptr<Object> Routines::getTriggerrer(ExecutionContext &ctx) const {
    return static_pointer_cast<Object>(ctx.triggerer);
}

shared_ptr<Object> Routines::getObject(const VariablesList &args, int index) const {
    return static_pointer_cast<Object>(isOutOfRange(args, index) ? nullptr : args[index].object);
}

shared_ptr<Object> Routines::getObjectOrCaller(const VariablesList &args, int index, ExecutionContext &ctx) const {
    return static_pointer_cast<Object>(isOutOfRange(args, index) ? ctx.caller : args[index].object);
}

shared_ptr<SpatialObject> Routines::getSpatialObject(const VariablesList &args, int index) const {
    return dynamic_pointer_cast<SpatialObject>(isOutOfRange(args, index) ? nullptr : args[index].object);
}

shared_ptr<SpatialObject> Routines::getSpatialObjectOrCaller(const VariablesList &args, int index, ExecutionContext &ctx) const {
    return dynamic_pointer_cast<SpatialObject>(isOutOfRange(args, index) ? ctx.caller : args[index].object);
}

shared_ptr<Creature> Routines::getCreature(const VariablesList &args, int index) const {
    return dynamic_pointer_cast<Creature>(isOutOfRange(args, index) ? nullptr : args[index].object);
}

shared_ptr<Creature> Routines::getCreatureOrCaller(const VariablesList &args, int index, ExecutionContext &ctx) const {
    return dynamic_pointer_cast<Creature>(isOutOfRange(args, index) ? ctx.caller : args[index].object);
}

shared_ptr<Door> Routines::getDoor(const VariablesList &args, int index) const {
    return dynamic_pointer_cast<Door>(isOutOfRange(args, index) ? nullptr : args[index].object);
}

shared_ptr<Item> Routines::getItem(const VariablesList &args, int index) const {
    return dynamic_pointer_cast<Item>(isOutOfRange(args, index) ? nullptr : args[index].object);
}

shared_ptr<Sound> Routines::getSound(const VariablesList &args, int index) const {
    return dynamic_pointer_cast<Sound>(isOutOfRange(args, index) ? nullptr : args[index].object);
}

shared_ptr<Effect> Routines::getEffect(const VariablesList &args, int index) const {
    return dynamic_pointer_cast<Effect>(isOutOfRange(args, index) ? nullptr : args[index].engineType);
}

shared_ptr<Event> Routines::getEvent(const VariablesList &args, int index) const {
    return dynamic_pointer_cast<Event>(isOutOfRange(args, index) ? nullptr : args[index].engineType);
}

shared_ptr<Location> Routines::getLocationEngineType(const VariablesList &args, int index) const {
    return dynamic_pointer_cast<Location>(isOutOfRange(args, index) ? nullptr : args[index].engineType);
}

shared_ptr<Talent> Routines::getTalent(const VariablesList &args, int index) const {
    return dynamic_pointer_cast<Talent>(isOutOfRange(args, index) ? nullptr : args[index].engineType);
}

shared_ptr<ExecutionContext> Routines::getAction(const VariablesList &args, int index) const {
    return args[index].context;
}

} // namespace game

} // namespace reone
