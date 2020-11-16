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

#include "../../common/log.h"

#include "../action/commandaction.h"
#include "../action/locationaction.h"
#include "../action/movetoobject.h"
#include "../action/startconversation.h"
#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::delayCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    float delay = args[0].floatValue;
    unique_ptr<CommandAction> action(new CommandAction(args[1].context));

    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    if (subject) {
        subject->actionQueue().delay(move(action), delay);
    }

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

Variable Routines::actionDoCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    if (subject) {
        unique_ptr<CommandAction> action(new CommandAction(args[0].context));
        subject->actionQueue().add(move(action));
    }
    return Variable();
}

Variable Routines::actionMoveToObject(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    bool run = args.size() >= 2 ? (args[1].intValue != 0) : false;
    float range = args.size() >= 3 ? args[2].floatValue : 1.0f;

    if (subject && object) {
        unique_ptr<MoveToObjectAction> action(new MoveToObjectAction(object, run, range));
        subject->actionQueue().add(move(action));
    }

    return Variable();
}

Variable Routines::actionStartConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));

    if (subject && object) {
        string dialogResRef((args.size() >= 2 && !args[1].strValue.empty()) ? args[1].strValue : subject->conversation());
        bool ignoreStartRange = args.size() >= 4 ? (args[4].intValue != 0) : false;

        unique_ptr<StartConversationAction> action(new StartConversationAction(object, dialogResRef, ignoreStartRange));
        subject->actionQueue().add(move(action));
    }

    return Variable();
}

Variable Routines::actionPauseConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    if (subject) {
        unique_ptr<Action> action(new Action(ActionType::PauseConversation));
        subject->actionQueue().add(move(action));
    }
    return Variable();
}

Variable Routines::actionResumeConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    if (subject) {
        unique_ptr<Action> action(new Action(ActionType::ResumeConversation));
        subject->actionQueue().add(move(action));
    }
    return Variable();
}

Variable Routines::actionOpenDoor(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));

    if (subject && object) {
        unique_ptr<ObjectAction> action(new ObjectAction(ActionType::OpenDoor, object));
        subject->actionQueue().add(move(action));
    }

    return Variable();
}

Variable Routines::actionCloseDoor(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));

    if (subject && object) {
        unique_ptr<ObjectAction> action(new ObjectAction(ActionType::CloseDoor, object));
        subject->actionQueue().add(move(action));
    }

    return Variable();
}

Variable Routines::clearAllActions(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    if (subject) {
        subject->clearAllActions();
    }
    return Variable();
}

Variable Routines::actionJumpToObject(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));

    if (subject && object) {
        subject->actionQueue().add(make_unique<ObjectAction>(ActionType::JumpToObject, object));
    }

    return Variable();
}

Variable Routines::actionJumpToLocation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Location> location(getLocationById(args[0].engineTypeId));

    if (subject && location) {
        subject->actionQueue().add(make_unique<LocationAction>(ActionType::JumpToLocation, location));
    }

    return Variable();
}

Variable Routines::actionForceMoveToObject(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    bool run = args.size() >= 2 ? (args[1].intValue != 0) : false;
    float range = args.size() >= 3 ? args[2].floatValue : 1.0f;

    if (subject && object) {
        subject->actionQueue().add(make_unique<MoveToObjectAction>(object, run, range));
    }

    return Variable();
}

Variable Routines::actionForceMoveToLocation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Location> location(getLocationById(args[0].engineTypeId));

    if (subject && location) {
        subject->actionQueue().add(make_unique<MoveToPointAction>(location->position()));
    }

    return Variable();
}

Variable Routines::jumpToObject(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));

    if (subject && object) {
        ActionQueue &actions = subject->actionQueue();
        actions.clear();
        actions.add(make_unique<ObjectAction>(ActionType::JumpToObject, object));
    }

    return Variable();
}

Variable Routines::jumpToLocation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> subject(getObjectById(ctx.callerId, ctx));
    shared_ptr<Location> location(getLocationById(args[0].engineTypeId));

    if (subject && location) {
        ActionQueue &actions = subject->actionQueue();
        actions.clear();
        actions.add(make_unique<LocationAction>(ActionType::JumpToLocation, location));
    }

    return Variable();
}

} // namespace game

} // namespace reone
