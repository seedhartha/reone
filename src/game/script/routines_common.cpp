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

#include "../../system/log.h"
#include "../../system/random.h"

#include "../action/commandaction.h"
#include "../action/movetoobject.h"
#include "../action/movetopoint.h"
#include "../action/startconversation.h"
#include "../game.h"
#include "../object/area.h"
#include "../object/creature.h"
#include "../object/door.h"

using namespace std;

using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

Variable RoutineManager::random(const vector<Variable> &args, ExecutionContext &ctx) {
    return reone::random(0, args[0].intValue - 1);
}

Variable RoutineManager::intToFloat(const vector<Variable> & args, ExecutionContext & ctx) {
    return static_cast<float>(args[0].intValue);
}

Variable RoutineManager::destroyObject(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    if (object) {
        shared_ptr<SpatialObject> spatial(dynamic_pointer_cast<SpatialObject>(object));
        if (spatial) {
            _game->module()->area()->destroyObject(*spatial);
        }
    } else {
        warn("Routine: object not found by id: " + to_string(objectId));
    }

    return Variable();
}

Variable RoutineManager::getEnteringObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.triggererId;
    return move(result);
}

Variable RoutineManager::getIsPC(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> player(_game->module()->area()->player());
    return Variable(args[0].objectId == player->id());
}

Variable RoutineManager::getIsObjectValid(const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable(args[0].objectId != kObjectInvalid);
}

Variable RoutineManager::getFirstPC(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> player(_game->module()->area()->player());

    Variable result(VariableType::Object);
    result.objectId = player->id();

    return move(result);
}

Variable RoutineManager::getObjectByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    string tag(args[0].strValue);
    if (tag.empty()) {
        tag = "party-leader";
    }
    int nth = args.size() >= 2 ? args[1].intValue : 0;
    shared_ptr<Object> object(_game->module()->area()->find(tag, nth));

    Variable result(VariableType::Object);
    result.objectId = object ? object->id() : kObjectInvalid;

    return move(result);
}

Variable RoutineManager::getWaypointByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(_game->module()->area()->find(args[0].strValue));

    Variable result(VariableType::Object);
    result.objectId = object ? object->id() : kObjectInvalid;

    return move(result);
}

Variable RoutineManager::getLevelByClass(const vector<Variable> &args, ExecutionContext &ctx) {
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
    int objectId = args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    Creature &creature = static_cast<Creature &>(*object);

    return Variable(static_cast<int>(creature.gender()));
}

Variable RoutineManager::getArea(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = _game->module()->area()->id();

    return move(result);
}

Variable RoutineManager::getItemInSlot(const vector<Variable> &args, ExecutionContext &ctx) {
    uint32_t objectId(args.size() > 1 ? args[1].objectId : kObjectSelf);
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    shared_ptr<Object> item;

    if (object) {
        Creature &creature = static_cast<Creature &>(*object);
        item = creature.getEquippedItem(static_cast<InventorySlot>(args[0].intValue));
    }
    Variable result(VariableType::Object);
    result.objectId = item ? item->id() : kObjectInvalid;

   return move(result);
}

Variable RoutineManager::getGlobalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getGlobalBoolean(args[0].strValue);
}

Variable RoutineManager::getGlobalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getGlobalNumber(args[0].strValue);
}

Variable RoutineManager::getLocalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getLocalBoolean(args[0].objectId, args[1].intValue);
}

Variable RoutineManager::getLocalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getLocalNumber(args[0].objectId, args[1].intValue);
}

Variable RoutineManager::setGlobalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    _game->setGlobalBoolean(args[0].strValue, args[1].intValue);
    return Variable();
}

Variable RoutineManager::setGlobalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    _game->setGlobalNumber(args[0].strValue, args[1].intValue);
    return Variable();
}

Variable RoutineManager::setLocalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    _game->setLocalBoolean(args[0].objectId, args[1].intValue, args[2].intValue);
    return Variable();
}

Variable RoutineManager::setLocalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    _game->setLocalNumber(args[0].objectId, args[1].intValue, args[2].intValue);
    return Variable();
}

Variable RoutineManager::delayCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    unique_ptr<CommandAction> action(new CommandAction(args[1].context));

    shared_ptr<Object> object(getObjectById(ctx.callerId, ctx));
    object->actionQueue().delay(move(action), args[0].floatValue);

    return Variable();
}

Variable RoutineManager::assignCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    unique_ptr<CommandAction> action(new CommandAction(args[1].context));

    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        object->actionQueue().add(move(action));
    }

    return Variable();
}

Variable RoutineManager::eventUserDefined(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Event);
    result.engineTypeId = _game->eventUserDefined(args[0].intValue);

    return move(result);
}

Variable RoutineManager::signalEvent(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(args[0].objectId, ctx));
    if (subject) {
        int eventNumber = _game->getUserDefinedEventNumber(args[1].engineTypeId);
        if (eventNumber != -1) {
            subject->runUserDefinedEvent(eventNumber);
        }
    } else {
        warn("Routine: object not found by id: " + to_string(args[0].objectId));
    }

    return Variable();
}

Variable RoutineManager::getUserDefinedEventNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    return ctx.userDefinedEventNumber;
}

Variable RoutineManager::actionDoCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject.get());
    if (creature) {
        unique_ptr<CommandAction> action(new CommandAction(args[0].context));
        creature->actionQueue().add(move(action));
    }

    return Variable();
}

Variable RoutineManager::actionMoveToObject(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    float distance = args.size() >= 2 ? args[2].floatValue : 1.0f;

    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(objectId, ctx));

    if (subject) {
        Creature &creature = static_cast<Creature &>(*subject);
        unique_ptr<MoveToObjectAction> action(new MoveToObjectAction(object, distance));
        creature.actionQueue().add(move(action));
    } else {
        warn("Routine: object not found: " + to_string(objectId));
    }

    return Variable();
}

Variable RoutineManager::actionStartConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject ? subject.get() : nullptr);

    int objectId = args[0].objectId == kObjectSelf ? ctx.callerId : args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));

    if (creature) {
        string dialogResRef((args.size() >= 2 && !args[1].strValue.empty()) ? args[1].strValue : creature->conversation());
        unique_ptr<StartConversationAction> action(new StartConversationAction(object, dialogResRef));
        creature->actionQueue().add(move(action));
    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable RoutineManager::actionPauseConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject ? subject.get() : nullptr);

    if (creature) {
        unique_ptr<Action> action(new Action(ActionType::PauseConversation));
        creature->actionQueue().add(move(action));
    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable RoutineManager::actionResumeConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject ? subject.get() : nullptr);

    if (creature) {
        unique_ptr<Action> action(new Action(ActionType::ResumeConversation));
        creature->actionQueue().add(move(action));
    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable RoutineManager::actionOpenDoor(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(objectId, ctx));

    if (subject) {
        Creature *creature = dynamic_cast<Creature *>(subject.get());
        if (creature) {
            unique_ptr<ObjectAction> action(new ObjectAction(ActionType::OpenDoor, object));
            creature->actionQueue().add(move(action));
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
    int objectId = args[0].objectId;
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(objectId, ctx));

    if (subject) {
        Creature *creature = dynamic_cast<Creature *>(subject.get());
        if (creature) {
            unique_ptr<ObjectAction> action(new ObjectAction(ActionType::CloseDoor, object));
            creature->actionQueue().add(move(action));
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

Variable RoutineManager::isAvailableCreature(const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = args[0].intValue;
    bool available = _game->party().isMemberAvailable(npc);

    return Variable(available);
}

Variable RoutineManager::addAvailableNPCByTemplate(const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = args[0].intValue;
    string blueprint(args[1].strValue);
    bool added = _game->party().addAvailableMember(npc, blueprint);

    return Variable(added);
}

Variable RoutineManager::showPartySelectionGUI(const vector<Variable> &args, ExecutionContext &ctx) {
    string exitScript(args.size() >= 1 ? args[0].strValue : "");
    int forceNpc1 = args.size() >= 2 ? args[1].intValue : -1;
    int forceNpc2 = args.size() >= 3 ? args[1].intValue : -1;

    PartySelection::Context partyCtx;
    partyCtx.exitScript = move(exitScript);
    partyCtx.forceNpc1 = forceNpc1;
    partyCtx.forceNpc2 = forceNpc2;

    _game->openPartySelection(partyCtx);

    return Variable();
}

} // namespace game

} // namespace reone
