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

#include "argutil.h"

#include "../../../../script/variable.h"

#include "../../../effect/effect.h"
#include "../../../event.h"
#include "../../../game.h"
#include "../../../location.h"
#include "../../../object/creature.h"
#include "../../../talent.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

bool getBool(const vector<Variable> &args, int index, bool defValue) {
    return isOutOfRange(args, index) ?
        defValue :
        static_cast<bool>(args[index].intValue);
}

int getInt(const vector<Variable> &args, int index, int defValue) {
    return isOutOfRange(args, index) ? defValue : args[index].intValue;
}

float getFloat(const vector<Variable> &args, int index, float defValue) {
    return isOutOfRange(args, index) ? defValue : args[index].floatValue;
}

string getString(const vector<Variable> &args, int index, string defValue) {
    return isOutOfRange(args, index) ? move(defValue) : args[index].strValue;
}

glm::vec3 getVector(const vector<Variable> &args, int index, glm::vec3 defValue) {
    return isOutOfRange(args, index) ? move(defValue) : args[index].vecValue;
}

shared_ptr<Object> getCaller(Game &game, ExecutionContext &ctx) {
    return game.getObjectById(ctx.callerId);
}

shared_ptr<SpatialObject> getCallerAsSpatial(Game &game, ExecutionContext &ctx) {
    return dynamic_pointer_cast<SpatialObject>(getCaller(game, ctx));
}

shared_ptr<Creature> getCallerAsCreature(Game &game, ExecutionContext &ctx) {
    return dynamic_pointer_cast<Creature>(getCaller(game, ctx));
}

shared_ptr<Object> getTriggerrer(Game &game, ExecutionContext &ctx) {
    return game.getObjectById(ctx.triggererId);
}

shared_ptr<Object> getObject(Game &game, const vector<Variable> &args, int index, ExecutionContext &ctx) {
    uint32_t objectId = isOutOfRange(args, index) ? kObjectInvalid : args[index].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.callerId;
    }
    return game.getObjectById(objectId);
}

shared_ptr<Object> getObjectOrCaller(Game &game, const vector<Variable> &args, int index, ExecutionContext &ctx) {
    uint32_t objectId = isOutOfRange(args, index) ? kObjectSelf : args[index].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.callerId;
    }
    return game.getObjectById(objectId);
}

shared_ptr<SpatialObject> getSpatialObject(Game &game, const vector<Variable> &args, int index, ExecutionContext &ctx) {
    return dynamic_pointer_cast<SpatialObject>(getObject(game, args, index, ctx));
}

shared_ptr<SpatialObject> getSpatialObjectOrCaller(Game &game, const vector<Variable> &args, int index, ExecutionContext &ctx) {
    return dynamic_pointer_cast<SpatialObject>(getObjectOrCaller(game, args, index, ctx));
}

shared_ptr<Creature> getCreature(Game &game, const vector<Variable> &args, int index, ExecutionContext &ctx) {
    return dynamic_pointer_cast<Creature>(getObject(game, args, index, ctx));
}

shared_ptr<Creature> getCreatureOrCaller(Game &game, const vector<Variable> &args, int index, ExecutionContext &ctx) {
    return dynamic_pointer_cast<Creature>(getObjectOrCaller(game, args, index, ctx));
}

shared_ptr<Door> getDoor(Game &game, const vector<Variable> &args, int index, ExecutionContext &ctx) {
    return dynamic_pointer_cast<Door>(getObject(game, args, index, ctx));
}

shared_ptr<Item> getItem(Game &game, const vector<Variable> &args, int index, ExecutionContext &ctx) {
    return dynamic_pointer_cast<Item>(getObject(game, args, index, ctx));
}

shared_ptr<Sound> getSound(Game &game, const vector<Variable> &args, int index, ExecutionContext &ctx) {
    return dynamic_pointer_cast<Sound>(getObject(game, args, index, ctx));
}

shared_ptr<Effect> getEffect(const vector<Variable> &args, int index) {
    return dynamic_pointer_cast<Effect>(isOutOfRange(args, index) ? nullptr : args[index].engineType);
}

shared_ptr<Event> getEvent(const vector<Variable> &args, int index) {
    return dynamic_pointer_cast<Event>(isOutOfRange(args, index) ? nullptr : args[index].engineType);
}

shared_ptr<Location> getLocationEngineType(const vector<Variable> &args, int index) {
    return dynamic_pointer_cast<Location>(isOutOfRange(args, index) ? nullptr : args[index].engineType);
}

shared_ptr<Talent> getTalent(const vector<Variable> &args, int index) {
    return dynamic_pointer_cast<Talent>(isOutOfRange(args, index) ? nullptr : args[index].engineType);
}

shared_ptr<ExecutionContext> getAction(const vector<Variable> &args, int index) {
    return args[index].context;
}

} // namespace game

} // namespace reone
