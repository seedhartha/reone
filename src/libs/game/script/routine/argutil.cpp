/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/script/routine/argutil.h"

#include "reone/script/executioncontext.h"

#include "reone/game/effect.h"
#include "reone/game/event.h"
#include "reone/game/game.h"
#include "reone/game/location.h"
#include "reone/game/object/creature.h"
#include "reone/game/object/door.h"
#include "reone/game/object/sound.h"
#include "reone/game/talent.h"

#include "reone/game/script/routine/context.h"

using namespace reone::script;

namespace reone {

namespace game {

static inline void throwIfInvalidObject(uint32_t objectId, const std::shared_ptr<Object> &object) {
    if (!object) {
        throw ArgumentException(str(boost::format("Id %u does not reference a valid object") % objectId));
    }
}

static inline void throwIfObjectNotCreature(const std::shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Creature) {
        throw ArgumentException(str(boost::format("Object %u is not a creature") % object->id()));
    }
}

static inline void throwIfObjectNotDoor(const std::shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Door) {
        throw ArgumentException(str(boost::format("Object %u is not a door") % object->id()));
    }
}

static inline void throwIfObjectNotPlaceable(const std::shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Placeable) {
        throw ArgumentException(str(boost::format("Object %u is not a placeable") % object->id()));
    }
}

static inline void throwIfObjectNotItem(const std::shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Item) {
        throw ArgumentException(str(boost::format("Object %u is not an item") % object->id()));
    }
}

static inline void throwIfObjectNotSound(const std::shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Sound) {
        throw ArgumentException(str(boost::format("Object %u is not a sound") % object->id()));
    }
}

static inline void throwIfObjectNotArea(const std::shared_ptr<Object> &object) {
    if (object->type() != ObjectType::Area) {
        throw ArgumentException(str(boost::format("Object %u is not an area") % object->id()));
    }
}

static inline void throwIfInvalidEffect(const std::shared_ptr<Effect> &effect) {
    if (!effect || effect->type() == EffectType::Invalid) {
        throw ArgumentException("Invalid effect");
    }
}

static inline void throwIfInvalidEvent(const std::shared_ptr<Event> &event) {
    if (!event) {
        throw ArgumentException("Invalid event");
    }
}

static inline void throwIfInvalidLocation(const std::shared_ptr<Location> &location) {
    if (!location) {
        throw ArgumentException("Invalid location");
    }
}

static inline void throwIfInvalidTalent(const std::shared_ptr<Talent> &talent) {
    if (!talent || talent->type() == TalentType::Invalid) {
        throw ArgumentException("Invalid talent");
    }
}

std::shared_ptr<Object> getCaller(const RoutineContext &ctx) {
    auto object = ctx.game.getObjectById(ctx.execution.callerId);
    throwIfInvalidObject(ctx.execution.callerId, object);
    return std::move(object);
}

std::shared_ptr<Object> getTriggerrer(const RoutineContext &ctx) {
    auto object = ctx.game.getObjectById(ctx.execution.triggererId);
    throwIfInvalidObject(ctx.execution.triggererId, object);
    return std::move(object);
}

int getInt(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Int, args[index].type);
    return args[index].intValue;
}

float getFloat(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Float, args[index].type);
    return args[index].floatValue;
}

std::string getString(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::String, args[index].type);
    return args[index].strValue;
}

glm::vec3 getVector(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Vector, args[index].type);
    return args[index].vecValue;
}

std::shared_ptr<Object> getObject(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Object, args[index].type);

    uint32_t objectId = args[index].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.execution.callerId;
    }
    auto object = ctx.game.getObjectById(objectId);
    throwIfInvalidObject(objectId, object);

    return std::move(object);
}

std::shared_ptr<Effect> getEffect(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Effect, args[index].type);
    auto effect = std::static_pointer_cast<Effect>(args[index].engineType);
    throwIfInvalidEffect(effect);
    return std::move(effect);
}

std::shared_ptr<Event> getEvent(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Event, args[index].type);
    auto event = std::static_pointer_cast<Event>(args[index].engineType);
    throwIfInvalidEvent(event);
    return std::move(event);
}

std::shared_ptr<Location> getLocationArgument(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Location, args[index].type);
    auto location = std::static_pointer_cast<Location>(args[index].engineType);
    throwIfInvalidLocation(location);
    return std::move(location);
}

std::shared_ptr<Talent> getTalent(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Talent, args[index].type);
    auto talent = std::static_pointer_cast<Talent>(args[index].engineType);
    throwIfInvalidTalent(talent);
    return std::move(talent);
}

std::shared_ptr<ExecutionContext> getAction(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Action, args[index].type);
    return args[index].context;
}

int getIntOrElse(const std::vector<Variable> &args, int index, int defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::Int, args[index].type);
    return args[index].intValue;
}

float getFloatOrElse(const std::vector<Variable> &args, int index, float defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::Float, args[index].type);
    return args[index].floatValue;
}

std::string getStringOrElse(const std::vector<Variable> &args, int index, std::string defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::String, args[index].type);
    return args[index].strValue;
}

glm::vec3 getVectorOrElse(const std::vector<Variable> &args, int index, glm::vec3 defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::Vector, args[index].type);
    return args[index].vecValue;
}

bool getIntAsBool(const std::vector<Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(VariableType::Int, args[index].type);
    return static_cast<bool>(args[index].intValue);
}

bool getIntAsBoolOrElse(const std::vector<Variable> &args, int index, bool defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(VariableType::Int, args[index].type);
    return static_cast<bool>(args[index].intValue);
}

std::shared_ptr<Object> getObjectOrNull(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return nullptr;
    } else {
        return getObject(args, index, ctx);
    }
}

std::shared_ptr<Object> getObjectOrCaller(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return getCaller(ctx);
    } else {
        return getObject(args, index, ctx);
    }
}

std::shared_ptr<Creature> getCallerAsCreature(const RoutineContext &ctx) {
    auto caller = getCaller(ctx);
    throwIfObjectNotCreature(caller);
    return std::static_pointer_cast<Creature>(std::move(caller));
}

std::shared_ptr<Creature> getObjectAsCreature(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfObjectNotCreature(object);
    return std::static_pointer_cast<Creature>(std::move(object));
}

std::shared_ptr<Creature> getObjectOrCallerAsCreature(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return getCallerAsCreature(ctx);
    } else {
        return getObjectAsCreature(args, index, ctx);
    }
}

std::shared_ptr<Door> getObjectAsDoor(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfObjectNotDoor(object);
    return std::static_pointer_cast<Door>(std::move(object));
}

std::shared_ptr<Placeable> getObjectAsPlaceable(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfObjectNotPlaceable(object);
    return std::static_pointer_cast<Placeable>(std::move(object));
}

std::shared_ptr<Item> getObjectAsItem(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfObjectNotItem(object);
    return std::static_pointer_cast<Item>(std::move(object));
}

std::shared_ptr<Item> getObjectAsItemOrNull(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return nullptr;
    } else {
        return getObjectAsItem(args, index, ctx);
    }
}

std::shared_ptr<Sound> getObjectAsSound(const std::vector<Variable> &args, int index, const RoutineContext &ctx) {
    auto object = getObject(args, index, ctx);
    throwIfObjectNotSound(object);
    return std::static_pointer_cast<Sound>(std::move(object));
}

std::shared_ptr<Area> getObjectAsArea(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx) {
    throwIfOutOfRange(args, 0);
    throwIfUnexpectedType(VariableType::Object, args[index].type);
    uint32_t objectId = args[index].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.execution.callerId;
    }
    auto object = ctx.game.getObjectById(objectId);
    throwIfObjectNotArea(object);
    return std::static_pointer_cast<Area>(object);
}

std::shared_ptr<Area> getObjectAsAreaOrCallerArea(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx) {
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
    return std::static_pointer_cast<Area>(object);
}

} // namespace game

} // namespace reone
