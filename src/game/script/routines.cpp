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

#include "../enginetype/event.h"
#include "../enginetype/location.h"

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
    int argCount = static_cast<int>(args.size());
    return index < argCount ? (args[index].intValue != 0) : defValue;
}

int Routines::getInt(const VariablesList &args, int index, int defValue) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? args[index].intValue : defValue;
}

float Routines::getFloat(const VariablesList &args, int index, float defValue) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? args[index].floatValue : defValue;
}

string Routines::getString(const VariablesList &args, int index, string defValue) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? args[index].strValue : move(defValue);
}

glm::vec3 Routines::getVector(const VariablesList &args, int index, glm::vec3 defValue) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? args[index].vecValue : move(defValue);
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
    int argCount = static_cast<int>(args.size());
    return index < argCount ? static_pointer_cast<Object>(args[index].object) : nullptr;
}

shared_ptr<Object> Routines::getObjectOrCaller(const VariablesList &args, int index, ExecutionContext &ctx) const {
    int argCount = static_cast<int>(args.size());
    return static_pointer_cast<Object>(index < argCount ? args[index].object : ctx.caller);
}

shared_ptr<SpatialObject> Routines::getSpatialObject(const VariablesList &args, int index) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? dynamic_pointer_cast<SpatialObject>(args[index].object) : nullptr;
}

shared_ptr<SpatialObject> Routines::getSpatialObjectOrCaller(const VariablesList &args, int index, ExecutionContext &ctx) const {
    int argCount = static_cast<int>(args.size());
    return dynamic_pointer_cast<SpatialObject>(index < argCount ? args[index].object : ctx.caller);
}

shared_ptr<Creature> Routines::getCreature(const VariablesList &args, int index) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? dynamic_pointer_cast<Creature>(args[index].object) : nullptr;
}

shared_ptr<Creature> Routines::getCreatureOrCaller(const VariablesList &args, int index, ExecutionContext &ctx) const {
    int argCount = static_cast<int>(args.size());
    return dynamic_pointer_cast<Creature>(index < argCount ? args[index].object : ctx.caller);
}

shared_ptr<Door> Routines::getDoor(const VariablesList &args, int index) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? dynamic_pointer_cast<Door>(args[index].object) : nullptr;
}

shared_ptr<Sound> Routines::getSound(const VariablesList &args, int index) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? dynamic_pointer_cast<Sound>(args[index].object) : nullptr;
}

shared_ptr<Location> Routines::getLocationEngineType(const VariablesList &args, int index) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? dynamic_pointer_cast<Location>(args[index].engineType) : nullptr;
}

shared_ptr<Event> Routines::getEvent(const VariablesList &args, int index) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? dynamic_pointer_cast<Event>(args[index].engineType) : nullptr;
}

shared_ptr<ExecutionContext> Routines::getAction(const VariablesList &args, int index) const {
    int argCount = static_cast<int>(args.size());
    if (index >= argCount) {
        throw out_of_range("index is out of range");
    }
    return args[index].context;
}

shared_ptr<Item> Routines::getItem(const VariablesList &args, int index) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? dynamic_pointer_cast<Item>(args[index].object) : nullptr;
}

shared_ptr<Effect> Routines::getEffect(const VariablesList &args, int index) const {
    int argCount = static_cast<int>(args.size());
    return index < argCount ? dynamic_pointer_cast<Effect>(args[index].engineType) : nullptr;
}

} // namespace game

} // namespace reone
