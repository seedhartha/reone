/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "SDL2/SDL_timer.h"

#include "../../core/log.h"
#include "../../core/random.h"

#include "../object/creature.h"
#include "../object/door.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable RoutineManager::random(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Int);
    return reone::random(0, args[0].intValue - 1);
}

Variable RoutineManager::intToFloat(const vector<Variable> & args, ExecutionContext & ctx) {
    assert(!args.empty() && args[0].type == VariableType::Int);
    return static_cast<float>(args[0].intValue);
}

Variable RoutineManager::getEnteringObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.triggererId;
    return move(result);
}

Variable RoutineManager::getIsPC(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);
    shared_ptr<Object> player(_callbacks->getPlayer());

    return Variable(args[0].objectId == player->id());
}

Variable RoutineManager::getIsObjectValid(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);
    return Variable(args[0].objectId != kObjectInvalid);
}

Variable RoutineManager::getFirstPC(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> player(_callbacks->getPlayer());

    Variable result(VariableType::Object);
    result.objectId = player->id();

    return move(result);
}

Variable RoutineManager::getObjectByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        !args.empty() &&
        args[0].type == VariableType::String &&
        (args.size() < 2 || args[1].type == VariableType::Int));

    string tag(args[0].strValue);
    if (tag.empty()) {
        tag = "party-leader";
    }
    int nth = args.size() >= 2 ? args[1].intValue : 0;
    shared_ptr<Object> object(_callbacks->getObjectByTag(tag, nth));

    Variable result(VariableType::Object);
    result.objectId = object ? object->id() : kObjectInvalid;

    return move(result);
}

Variable RoutineManager::getWaypointByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::String);
    shared_ptr<Object> object(_callbacks->getWaypointByTag(args[0].strValue));

    Variable result(VariableType::Object);
    result.objectId = object ? object->id() : kObjectInvalid;

    return move(result);
}

Variable RoutineManager::getLevelByClass(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        !args.empty() &&
        args[0].type == VariableType::Int &&
        args[1].type == VariableType::Object);

    ClassType clazz = static_cast<ClassType>(args[0].intValue);

    int objectId = args.size() < 2 ? kObjectSelf : args[1].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    if (!object) {
        warn("Routine: object not found: " + to_string(objectId));
        return 0;
    }

    Creature &creature = static_cast<Creature &>(*object);

    return Variable(creature.getClassLevel(clazz));
}

Variable RoutineManager::getGender(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);

    int objectId = args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    Creature &creature = static_cast<Creature &>(*object);

    return Variable(static_cast<int>(creature.gender()));
}

Variable RoutineManager::getArea(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);

    Variable result(VariableType::Object);
    result.objectId = kObjectArea;

    return move(result);
}

Variable RoutineManager::getGlobalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::String);
    return _callbacks->getGlobalBoolean(args[0].strValue);
}

Variable RoutineManager::getGlobalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::String);
    return _callbacks->getGlobalNumber(args[0].strValue);
}

Variable RoutineManager::getLocalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Object &&
        args[1].type == VariableType::Int);

    return _callbacks->getLocalBoolean(args[0].objectId, args[1].intValue);
}

Variable RoutineManager::getLocalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Object &&
        args[1].type == VariableType::Int);

    return _callbacks->getLocalNumber(args[0].objectId, args[1].intValue);
}

Variable RoutineManager::setGlobalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::String &&
        args[1].type == VariableType::Int);

    _callbacks->setGlobalBoolean(args[0].strValue, args[1].intValue);

    return Variable();
}

Variable RoutineManager::setGlobalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::String &&
        args[1].type == VariableType::Int);

    _callbacks->setGlobalNumber(args[0].strValue, args[1].intValue);

    return Variable();
}

Variable RoutineManager::setLocalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 3 &&
        args[0].type == VariableType::Object &&
        args[1].type == VariableType::Int && args[1].intValue >= 0 && args[1].intValue <= 63 &&
        args[2].type == VariableType::Int);

    _callbacks->setLocalBoolean(args[0].objectId, args[1].intValue, args[2].intValue);

    return Variable();
}

Variable RoutineManager::setLocalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 3 &&
        args[0].type == VariableType::Object &&
        args[1].type == VariableType::Int &&
        args[2].type == VariableType::Int);

    _callbacks->setLocalNumber(args[0].objectId, args[1].intValue, args[2].intValue);

    return Variable();
}

Variable RoutineManager::delayCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Float &&
        args[1].type == VariableType::Action);

    uint32_t timestamp = SDL_GetTicks() + static_cast<int>(args[0].floatValue * 1000.0f);
    _callbacks->delayCommand(timestamp, args[1].context);

    return Variable();
}

Variable RoutineManager::assignCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Object &&
        args[1].type == VariableType::Action);

    ExecutionContext newCtx(args[1].context);
    newCtx.callerId = args[0].objectId;
    newCtx.triggererId = kObjectInvalid;

    _callbacks->delayCommand(SDL_GetTicks(), move(newCtx));

    return Variable();
}

Variable RoutineManager::eventUserDefined(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Int);

    Variable result(VariableType::Event);
    result.engineTypeId = _callbacks->eventUserDefined(args[0].intValue);

    return move(result);
}

Variable RoutineManager::signalEvent(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Object &&
        args[1].type == VariableType::Event);

    if (args[0].objectId == kObjectArea ||
        (args[0].objectId == kObjectSelf && ctx.callerId == kObjectArea)) {

        _callbacks->signalEvent(args[1].engineTypeId);
    
    } else {
        warn("Routine: invalid callee: " + to_string(args[0].objectId));
    }

    return Variable();
}

Variable RoutineManager::getUserDefinedEventNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(args.empty());
    return ctx.userDefinedEventNumber;
}

Variable RoutineManager::actionDoCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Action);

    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject.get());
    if (creature) {
        Creature::Action action(Creature::ActionType::DoCommand, args[0].context);
        creature->enqueueAction(move(action));
    }

    return Variable();
}

Variable RoutineManager::actionMoveToObject(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        !args.empty() && args[0].type == VariableType::Object &&
        (args.size() < 2 || args[1].type == VariableType::Int) &&
        (args.size() < 3 || args[2].type == VariableType::Float));

    int objectId = args[0].objectId;
    float distance = args.size() >= 2 ? args[2].floatValue : 1.0f;

    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    if (subject) {
        Creature &creature = static_cast<Creature &>(*subject);
        Creature::Action action(Creature::ActionType::MoveToPoint, object, distance);
        creature.enqueueAction(move(action));
    } else {
        warn("Routine: object not found: " + to_string(objectId));
    }

    return Variable();
}

Variable RoutineManager::actionStartConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);

    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject ? subject.get() : nullptr);

    int objectId = args[0].objectId == kObjectSelf ? ctx.callerId : args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));

    if (creature) {
        Creature::Action action(Creature::ActionType::StartConversation, ctx);
        action.object = object;
        action.resRef = (args.size() >= 2 && !args[1].strValue.empty()) ? args[1].strValue : creature->conversation();

        creature->enqueueAction(move(action));

    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable RoutineManager::actionPauseConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(args.empty());

    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject ? subject.get() : nullptr);

    if (creature) {
        Creature::Action action(Creature::ActionType::PauseConversation, ctx);
        creature->enqueueAction(move(action));
    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable RoutineManager::actionResumeConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(args.empty());

    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject ? subject.get() : nullptr);

    if (creature) {
        Creature::Action action(Creature::ActionType::ResumeConversation, ctx);
        creature->enqueueAction(move(action));
    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable RoutineManager::actionOpenDoor(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);

    int objectId = args[0].objectId;
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    if (subject) {
        Creature *creature = dynamic_cast<Creature *>(subject.get());
        if (creature) {
            Creature::Action action(Creature::ActionType::OpenDoor, object, 1.0f);
            creature->enqueueAction(move(action));
        }
        Door *door = dynamic_cast<Door *>(subject.get());
        if (door) {
            door->open(nullptr);
        }
    } else {
        warn("Routine: object not found: " + to_string(objectId));
    }

    return Variable();
}

Variable RoutineManager::actionCloseDoor(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);

    int objectId = args[0].objectId;
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    if (subject) {
        Creature *creature = dynamic_cast<Creature *>(subject.get());
        if (creature) {
            Creature::Action action(Creature::ActionType::CloseDoor, object, 1.0f);
            creature->enqueueAction(move(action));
        }
        Door *door = dynamic_cast<Door *>(subject.get());
        if (door) {
            door->close(nullptr);
        }
    } else {
        warn("Routine: object not found: " + to_string(objectId));
    }

    return Variable();
}

} // namespace game

} // namespace reone
