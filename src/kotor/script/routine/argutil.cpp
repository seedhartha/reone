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

#include "../../../script/executioncontext.h"

#include "../../../game/effect.h"
#include "../../../game/event.h"
#include "../../../game/game.h"
#include "../../../game/location.h"
#include "../../../game/object/creature.h"
#include "../../../game/object/door.h"
#include "../../../game/object/sound.h"
#include "../../../game/talent.h"

#include "context.h"

using namespace std;

using namespace reone::game;
using namespace reone::script;

namespace reone {

namespace kotor {

static inline void throwIfInvalidObject(uint32_t objectId, const shared_ptr<Object> &object) {
    if (!object) {
        throw ArgumentException(str(boost::format("Id %u does not reference a valid object") % objectId));
    }
}

static inline void throwIfNotSpatialObject(const shared_ptr<Object> &object) {
    switch (object->type()) {
    case ObjectType::Creature:
    case ObjectType::Trigger:
    case ObjectType::Door:
    case ObjectType::Waypoint:
    case ObjectType::Placeable:
    case ObjectType::Encounter:
    case ObjectType::Sound:
    case ObjectType::Camera:
        break;
    default:
        throw ArgumentException(str(boost::format("Object %u is not a spatial object") % object->id()));
    }
}

static inline void throwIfNotCreature(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Creature) {
        throw ArgumentException(str(boost::format("Object %u is not a creature") % object->id()));
    }
}

static inline void throwIfNotDoor(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Door) {
        throw ArgumentException(str(boost::format("Object %u is not a door") % object->id()));
    }
}

static inline void throwIfNotItem(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Item) {
        throw ArgumentException(str(boost::format("Object %u is not an item") % object->id()));
    }
}

static inline void throwIfNotSound(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Sound) {
        throw ArgumentException(str(boost::format("Object %u is not a sound") % object->id()));
    }
}

shared_ptr<Object> getCaller(const RoutineContext &ctx) {
    auto object = ctx.game.getObjectById(ctx.execution.callerId);
    throwIfInvalidObject(ctx.execution.callerId, object);
    return move(object);
}

shared_ptr<Object> getTriggerrer(const RoutineContext &ctx) {
    auto object = ctx.game.getObjectById(ctx.execution.triggererId);
    throwIfInvalidObject(ctx.execution.triggererId, object);
    return move(object);
}

int getInt(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Int, args[index].type);
    return args[index].intValue;
}

float getFloat(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Float, args[index].type);
    return args[index].floatValue;
}

string getString(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::String, args[index].type);
    return args[index].strValue;
}

glm::vec3 getVector(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Vector, args[index].type);
    return args[index].vecValue;
}

shared_ptr<Object> getObject(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Object, args[index].type);

    uint32_t objectId = args[index].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.execution.callerId;
    }
    auto object = ctx.game.getObjectById(objectId);
    throwIfInvalidObject(objectId, object);

    return move(object);
}

shared_ptr<Effect> getEffect(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Effect, args[index].type);
    return static_pointer_cast<Effect>(args[index].engineType);
}

shared_ptr<Event> getEvent(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Event, args[index].type);
    return static_pointer_cast<Event>(args[index].engineType);
}

shared_ptr<Location> getLocationArgument(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Location, args[index].type);
    return static_pointer_cast<Location>(args[index].engineType);
}

shared_ptr<Talent> getTalent(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Talent, args[index].type);
    return static_pointer_cast<Talent>(args[index].engineType);
}

shared_ptr<ExecutionContext> getAction(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Action, args[index].type);
    return args[index].context;
}

int getIntOrElse(const vector<Variable> &args, int index, int defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::Int, args[index].type);
    return args[index].intValue;
}

float getFloatOrElse(const vector<Variable> &args, int index, float defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::Float, args[index].type);
    return args[index].floatValue;
}

string getStringOrElse(const vector<Variable> &args, int index, string defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::String, args[index].type);
    return args[index].strValue;
}

glm::vec3 getVectorOrElse(const vector<Variable> &args, int index, glm::vec3 defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::Vector, args[index].type);
    return args[index].vecValue;
}

bool getIntAsBool(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Int, args[index].type);
    return static_cast<bool>(args[index].intValue);
}

bool getIntAsBoolOrElse(const vector<Variable> &args, int index, bool defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::Int, args[index].type);
    return static_cast<bool>(args[index].intValue);
}

shared_ptr<Object> getObjectOrCaller(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return getCaller(ctx);
    } else {
        return getObject(args, index, ctx);
    }
}

shared_ptr<SpatialObject> getCallerAsSpatialObject(const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    throwIfNotSpatialObject(caller);
    return static_pointer_cast<SpatialObject>(move(caller));
}

shared_ptr<SpatialObject> getObjectAsSpatialObject(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfNotSpatialObject(object);
    return static_pointer_cast<SpatialObject>(move(object));
}

shared_ptr<SpatialObject> getObjectOrCallerAsSpatialObject(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return getCallerAsSpatialObject(ctx);
    } else {
        return getObjectAsSpatialObject(args, index, ctx);
    }
}

shared_ptr<Creature> getCallerAsCreature(const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    throwIfNotCreature(caller);
    return static_pointer_cast<Creature>(move(caller));
}

shared_ptr<Creature> getObjectAsCreature(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfNotCreature(object);
    return static_pointer_cast<Creature>(move(object));
}

shared_ptr<Creature> getObjectOrCallerAsCreature(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return getCallerAsCreature(ctx);
    } else {
        return getObjectAsCreature(args, index, ctx);
    }
}

shared_ptr<Door> getObjectAsDoor(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfNotDoor(object);
    return static_pointer_cast<Door>(move(object));
}

shared_ptr<Item> getObjectAsItem(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfNotItem(object);
    return static_pointer_cast<Item>(move(object));
}

shared_ptr<Sound> getObjectAsSound(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfNotSound(object);
    return static_pointer_cast<Sound>(move(object));
}

} // namespace kotor

} // namespace reone
