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

Variable Routines::random(const vector<Variable> &args, ExecutionContext &ctx) {
    return reone::random(0, args[0].intValue - 1);
}

Variable Routines::intToFloat(const vector<Variable> & args, ExecutionContext & ctx) {
    return static_cast<float>(args[0].intValue);
}

Variable Routines::destroyObject(const vector<Variable> &args, ExecutionContext &ctx) {
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

Variable Routines::getEnteringObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.triggererId;
    return move(result);
}

Variable Routines::getIsPC(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> player(_game->party().player());
    return Variable(args[0].objectId == player->id());
}

Variable Routines::getIsObjectValid(const vector<Variable> &args, ExecutionContext &ctx) {
    return Variable(args[0].objectId != kObjectInvalid);
}

Variable Routines::getFirstPC(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> player(_game->party().player());

    Variable result(VariableType::Object);
    result.objectId = player->id();

    return move(result);
}

Variable Routines::getObjectByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    string tag(args[0].strValue);
    if (tag.empty()) {
        tag = "PLAYER";
    }
    int nth = args.size() >= 2 ? args[1].intValue : 0;
    shared_ptr<Object> object(_game->module()->area()->find(tag, nth));

    Variable result(VariableType::Object);
    result.objectId = object ? object->id() : kObjectInvalid;

    return move(result);
}

Variable Routines::getWaypointByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> object(_game->module()->area()->find(args[0].strValue));

    Variable result(VariableType::Object);
    result.objectId = object ? object->id() : kObjectInvalid;

    return move(result);
}

Variable Routines::getLevelByClass(const vector<Variable> &args, ExecutionContext &ctx) {
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

Variable Routines::getGender(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    Creature &creature = static_cast<Creature &>(*object);

    return Variable(static_cast<int>(creature.gender()));
}

Variable Routines::getArea(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = _game->module()->area()->id();

    return move(result);
}

Variable Routines::getItemInSlot(const vector<Variable> &args, ExecutionContext &ctx) {
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

Variable Routines::getGlobalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getGlobalBoolean(args[0].strValue);
}

Variable Routines::getGlobalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getGlobalNumber(args[0].strValue);
}

Variable Routines::getLocalBoolean(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getLocalBoolean(args[0].objectId, args[1].intValue);
}

Variable Routines::getLocalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    return _game->getLocalNumber(args[0].objectId, args[1].intValue);
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
    _game->setLocalBoolean(args[0].objectId, args[1].intValue, args[2].intValue);
    return Variable();
}

Variable Routines::setLocalNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    _game->setLocalNumber(args[0].objectId, args[1].intValue, args[2].intValue);
    return Variable();
}

Variable Routines::delayCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    unique_ptr<CommandAction> action(new CommandAction(args[1].context));

    shared_ptr<Object> object(getObjectById(ctx.callerId, ctx));
    object->actionQueue().delay(move(action), args[0].floatValue);

    return Variable();
}

Variable Routines::assignCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    unique_ptr<CommandAction> action(new CommandAction(args[1].context));

    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        object->actionQueue().add(move(action));
    }

    return Variable();
}

Variable Routines::eventUserDefined(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Event);
    result.engineTypeId = _game->eventUserDefined(args[0].intValue);

    return move(result);
}

Variable Routines::signalEvent(const vector<Variable> &args, ExecutionContext &ctx) {
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

Variable Routines::getUserDefinedEventNumber(const vector<Variable> &args, ExecutionContext &ctx) {
    return ctx.userDefinedEventNumber;
}

Variable Routines::actionDoCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject.get());
    if (creature) {
        unique_ptr<CommandAction> action(new CommandAction(args[0].context));
        creature->actionQueue().add(move(action));
    }

    return Variable();
}

Variable Routines::actionMoveToObject(const vector<Variable> &args, ExecutionContext &ctx) {
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

Variable Routines::actionStartConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    Creature *creature = dynamic_cast<Creature *>(subject ? subject.get() : nullptr);

    int objectId = args[0].objectId == kObjectSelf ? ctx.callerId : args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));

    if (creature) {
        string dialogResRef((args.size() >= 2 && !args[1].strValue.empty()) ? args[1].strValue : creature->conversation());
        bool ignoreStartRange = args.size() >= 4 ? (args[4].intValue != 0) : false;
        unique_ptr<StartConversationAction> action(new StartConversationAction(subject, dialogResRef, ignoreStartRange));
        creature->actionQueue().add(move(action));
    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable Routines::actionPauseConversation(const vector<Variable> &args, ExecutionContext &ctx) {
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

Variable Routines::actionResumeConversation(const vector<Variable> &args, ExecutionContext &ctx) {
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

Variable Routines::actionOpenDoor(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(objectId, ctx));

    if (subject) {
        Creature *creature = dynamic_cast<Creature *>(subject.get());
        if (creature) {
            unique_ptr<ObjectAction> action(new ObjectAction(ActionType::OpenDoor, object));
            creature->actionQueue().add(move(action));
        } else {
            Door *door = dynamic_cast<Door *>(subject.get());
            if (door) {
                door->open(nullptr);
            }
        }
    } else {
        warn("Routine: object not found: " + to_string(objectId));
    }

    return Variable();
}

Variable Routines::actionCloseDoor(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(objectId, ctx));

    if (subject) {
        Creature *creature = dynamic_cast<Creature *>(subject.get());
        if (creature) {
            unique_ptr<ObjectAction> action(new ObjectAction(ActionType::CloseDoor, object));
            creature->actionQueue().add(move(action));
        } else {
            Door *door = dynamic_cast<Door *>(subject.get());
            if (door) {
                door->close(nullptr);
            }
        }
    } else {
        warn("Routine: object not found: " + to_string(objectId));
    }

    return Variable();
}

Variable Routines::isAvailableCreature(const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = args[0].intValue;
    bool available = _game->party().isMemberAvailable(npc);

    return Variable(available);
}

Variable Routines::addAvailableNPCByTemplate(const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = args[0].intValue;
    string blueprint(args[1].strValue);
    bool added = _game->party().addAvailableMember(npc, blueprint);

    return Variable(added);
}

Variable Routines::showPartySelectionGUI(const vector<Variable> &args, ExecutionContext &ctx) {
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
