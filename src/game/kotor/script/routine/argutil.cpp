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

#include "../../../../script/executioncontext.h"

#include "../../../effect.h"
#include "../../../event.h"
#include "../../../game.h"
#include "../../../location.h"
#include "../../../object/creature.h"
#include "../../../object/door.h"
#include "../../../object/sound.h"
#include "../../../talent.h"

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

static inline void throwIfObjectNotCreature(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Creature) {
        throw ArgumentException(str(boost::format("Object %u is not a creature") % object->id()));
    }
}

static inline void throwIfObjectNotDoor(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Door) {
        throw ArgumentException(str(boost::format("Object %u is not a door") % object->id()));
    }
}

static inline void throwIfObjectNotPlaceable(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Placeable) {
        throw ArgumentException(str(boost::format("Object %u is not a placeable") % object->id()));
    }
}

static inline void throwIfObjectNotItem(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Item) {
        throw ArgumentException(str(boost::format("Object %u is not an item") % object->id()));
    }
}

static inline void throwIfObjectNotSound(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Sound) {
        throw ArgumentException(str(boost::format("Object %u is not a sound") % object->id()));
    }
}

static inline void throwIfObjectNotArea(const shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Area) {
        throw ArgumentException(str(boost::format("Object %u is not an area") % object->id()));
    }
}

static inline void throwIfInvalidEffect(const shared_ptr<Effect> &effect) {
    if (!effect || effect->type() == EffectType::Invalid) {
        throw ArgumentException("Invalid effect");
    }
}

static inline void throwIfInvalidEvent(const shared_ptr<Event> &event) {
    if (!event) {
        throw ArgumentException("Invalid event");
    }
}

static inline void throwIfInvalidLocation(const shared_ptr<Location> &location) {
    if (!location) {
        throw ArgumentException("Invalid location");
    }
}

static inline void throwIfInvalidTalent(const shared_ptr<Talent> &talent) {
    if (!talent || talent->type() == TalentType::Invalid) {
        throw ArgumentException("Invalid talent");
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
    auto effect = static_pointer_cast<Effect>(args[index].engineType);
    throwIfInvalidEffect(effect);
    return move(effect);
}

shared_ptr<Event> getEvent(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Event, args[index].type);
    auto event = static_pointer_cast<Event>(args[index].engineType);
    throwIfInvalidEvent(event);
    return move(event);
}

shared_ptr<Location> getLocationArgument(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Location, args[index].type);
    auto location = static_pointer_cast<Location>(args[index].engineType);
    throwIfInvalidLocation(location);
    return move(location);
}

shared_ptr<Talent> getTalent(const vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Talent, args[index].type);
    auto talent = static_pointer_cast<Talent>(args[index].engineType);
    throwIfInvalidTalent(talent);
    return move(talent);
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

shared_ptr<Object> getObjectOrNull(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return nullptr;
    } else {
        return getObject(args, index, ctx);
    }
}

shared_ptr<Object> getObjectOrCaller(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return getCaller(ctx);
    } else {
        return getObject(args, index, ctx);
    }
}

shared_ptr<Creature> getCallerAsCreature(const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    throwIfObjectNotCreature(caller);
    return static_pointer_cast<Creature>(move(caller));
}

shared_ptr<Creature> getObjectAsCreature(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfObjectNotCreature(object);
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
    throwIfObjectNotDoor(object);
    return static_pointer_cast<Door>(move(object));
}

shared_ptr<Placeable> getObjectAsPlaceable(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfObjectNotPlaceable(object);
    return static_pointer_cast<Placeable>(move(object));
}

shared_ptr<Item> getObjectAsItem(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfObjectNotItem(object);
    return static_pointer_cast<Item>(move(object));
}

shared_ptr<Item> getObjectAsItemOrNull(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return nullptr;
    } else {
        return getObjectAsItem(args, index, ctx);
    }
}

shared_ptr<Sound> getObjectAsSound(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfObjectNotSound(object);
    return static_pointer_cast<Sound>(move(object));
}

shared_ptr<Area> getObjectAsArea(const vector<script::Variable> &args, int index, const RoutineContext &ctx) {
    throwIfOutOfRange(args, 0);
    throwIfUnexpectedType(VariableType::Object, args[index].type);
    uint32_t objectId = args[index].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.execution.callerId;
    }
    auto object = ctx.game.getObjectById(objectId);
    throwIfObjectNotArea(object);
    return static_pointer_cast<Area>(object);
}

shared_ptr<Area> getObjectAsAreaOrCallerArea(const vector<script::Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return ctx.game.module()->area();
    }
    throwIfUnexpectedType(VariableType::Object, args[index].type);
    uint32_t objectId = args[index].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.execution.callerId;
    } else if (objectId == kObjectInvalid) {
        return ctx.game.module()->area();
    }
    auto object = ctx.game.getObjectById(objectId);
    throwIfObjectNotArea(object);
    return static_pointer_cast<Area>(object);
}

} // namespace kotor

} // namespace reone
