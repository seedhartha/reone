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

#include "../../../../script/exception/argument.h"
#include "../../../../script/executioncontext.h"

#include "../../../effect/effect.h"
#include "../../../event.h"
#include "../../../game.h"
#include "../../../location.h"
#include "../../../object/creature.h"
#include "../../../object/door.h"
#include "../../../object/sound.h"
#include "../../../talent.h"

#include "../context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

bool getBool(const vector<Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    return static_cast<bool>(args[index].intValue);
}

int getInt(const vector<Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    return args[index].intValue;
}

float getFloat(const vector<Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    return args[index].floatValue;
}

string getString(const vector<Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    return args[index].strValue;
}

glm::vec3 getVector(const vector<Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    return args[index].vecValue;
}

bool getBoolOrElse(const vector<Variable> &args, int index, bool defValue) {
    return isOutOfRange(args, index) ? defValue : static_cast<bool>(args[index].intValue);
}

int getIntOrElse(const vector<Variable> &args, int index, int defValue) {
    return isOutOfRange(args, index) ? defValue : args[index].intValue;
}

float getFloatOrElse(const vector<Variable> &args, int index, float defValue) {
    return isOutOfRange(args, index) ? defValue : args[index].floatValue;
}

string getStringOrElse(const vector<Variable> &args, int index, string defValue) {
    return isOutOfRange(args, index) ? move(defValue) : args[index].strValue;
}

glm::vec3 getVectorOrElse(const vector<Variable> &args, int index, glm::vec3 defValue) {
    return isOutOfRange(args, index) ? move(defValue) : args[index].vecValue;
}

shared_ptr<Object> getCaller(const RoutineContext &ctx) {
    shared_ptr<Object> result(ctx.game.getObjectById(ctx.execution.callerId));
    if (!result) {
        throw ArgumentException("Caller is not a valid object: " + to_string(ctx.execution.callerId));
    }
    return move(result);
}

shared_ptr<SpatialObject> getCallerAsSpatial(const RoutineContext &ctx) {
    shared_ptr<Object> caller(getCaller(ctx));
    shared_ptr<SpatialObject> spatial(dynamic_pointer_cast<SpatialObject>(caller));
    if (!spatial) {
        throw ArgumentException("Caller is not a valid spatial object: " + to_string(caller->id()));
    }
    return move(spatial);
}

shared_ptr<Creature> getCallerAsCreature(const RoutineContext &ctx) {
    shared_ptr<Object> caller(getCaller(ctx));
    shared_ptr<Creature> creature(dynamic_pointer_cast<Creature>(caller));
    if (!creature) {
        throw ArgumentException("Caller is not a valid creature: " + to_string(caller->id()));
    }
    return move(creature);
}

shared_ptr<Object> getTriggerrer(const RoutineContext &ctx) {
    shared_ptr<Object> result(ctx.game.getObjectById(ctx.execution.triggererId));
    if (!result) {
        throw ArgumentException("Triggerrer is not a valid object: " + to_string(ctx.execution.triggererId));
    }
    return move(result);
}

shared_ptr<Object> getObject(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    uint32_t objectId = args[index].objectId;
    if (objectId == kObjectSelf) {
        objectId = ctx.execution.callerId;
    }
    shared_ptr<Object> result(ctx.game.getObjectById(objectId));
    if (!result) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid object: %d") % index % objectId));
    }
    return move(result);
}

shared_ptr<Object> getObjectOrCaller(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return getCaller(ctx);
    }
    return getObject(args, index, ctx);
}

shared_ptr<SpatialObject> getSpatialObject(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    shared_ptr<Object> object(getObject(args, index, ctx));
    shared_ptr<SpatialObject> spatial(dynamic_pointer_cast<SpatialObject>(object));
    if (!spatial) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid spatial object: %d") % index % object->id()));
    }
    return move(spatial);
}

shared_ptr<SpatialObject> getSpatialObjectOrCaller(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return getCallerAsSpatial(ctx);
    }
    return getSpatialObject(args, index, ctx);
}

shared_ptr<Creature> getCreature(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    shared_ptr<Object> object(getObject(args, index, ctx));
    shared_ptr<Creature> creature(dynamic_pointer_cast<Creature>(object));
    if (!creature) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid creature: %d") % index % object->id()));
    }
    return move(creature);
}

shared_ptr<Creature> getCreatureOrCaller(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    if (isOutOfRange(args, index)) {
        return getCallerAsCreature(ctx);
    }
    return getCreature(args, index, ctx);
}

shared_ptr<Door> getDoor(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    shared_ptr<Object> object(getObject(args, index, ctx));
    shared_ptr<Door> door(dynamic_pointer_cast<Door>(object));
    if (!door) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid door: %d") % index % object->id()));
    }
    return move(door);
}

shared_ptr<Item> getItem(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    shared_ptr<Object> object(getObject(args, index, ctx));
    shared_ptr<Item> item(dynamic_pointer_cast<Item>(object));
    if (!item) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid item: %d") % index % object->id()));
    }
    return move(item);
}

shared_ptr<Sound> getSound(const vector<Variable> &args, int index, const RoutineContext &ctx) {
    shared_ptr<Object> object(getObject(args, index, ctx));
    shared_ptr<Sound> sound(dynamic_pointer_cast<Sound>(object));
    if (!sound) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid sound: %d") % index % object->id()));
    }
    return move(sound);
}

shared_ptr<Effect> getEffect(const vector<Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    shared_ptr<Effect> result(dynamic_pointer_cast<Effect>(args[index].engineType));
    if (!result) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid effect") % index));
    }
    return move(result);
}

shared_ptr<Event> getEvent(const vector<Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    shared_ptr<Event> result(dynamic_pointer_cast<Event>(args[index].engineType));
    if (!result) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid event") % index));
    }
    return move(result);
}

shared_ptr<Location> getLocationEngineType(const vector<Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    shared_ptr<Location> result(dynamic_pointer_cast<Location>(args[index].engineType));
    if (!result) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid location") % index));
    }
    return move(result);
}

shared_ptr<Talent> getTalent(const vector<Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw ArgumentException(str(boost::format("Argument %d is out of range") % index));
    }
    shared_ptr<Talent> result(dynamic_pointer_cast<Talent>(args[index].engineType));
    if (!result) {
        throw ArgumentException(str(boost::format("Argument %d is not a valid talent") % index));
    }
    return move(result);
}

shared_ptr<ExecutionContext> getAction(const vector<Variable> &args, int index) {
    return ensurePresent(args[index].context, "context");
}

} // namespace game

} // namespace reone
