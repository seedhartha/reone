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
#include "../action/movetoobject.h"
#include "../action/startconversation.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

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

Variable Routines::actionDoCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> actor(getObjectById(ctx.callerId, ctx));
    if (actor) {
        unique_ptr<CommandAction> action(new CommandAction(args[0].context));
        actor->actionQueue().add(move(action));
    }

    return Variable();
}

Variable Routines::actionMoveToObject(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    bool run = args.size() >= 2 ? (args[1].intValue != 0) : false;
    float distance = args.size() >= 3 ? args[2].floatValue : 1.0f;

    shared_ptr<Object> actor(getObjectById(ctx.callerId, ctx));
    if (actor) {
        shared_ptr<Object> object(getObjectById(objectId, ctx));
        unique_ptr<MoveToObjectAction> action(new MoveToObjectAction(object, run, distance));
        actor->actionQueue().add(move(action));
    } else {
        warn("Routine: object not found: " + to_string(objectId));
    }

    return Variable();
}

Variable Routines::actionStartConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> actor(getObjectById(ctx.callerId, ctx));
    if (actor) {
        int objectId = args[0].objectId == kObjectSelf ? ctx.callerId : args[0].objectId;
        shared_ptr<Object> object(getObjectById(objectId, ctx));

        string dialogResRef((args.size() >= 2 && !args[1].strValue.empty()) ? args[1].strValue : actor->conversation());
        bool ignoreStartRange = args.size() >= 4 ? (args[4].intValue != 0) : false;

        unique_ptr<StartConversationAction> action(new StartConversationAction(object, dialogResRef, ignoreStartRange));
        actor->actionQueue().add(move(action));
    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable Routines::actionPauseConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> actor(getObjectById(ctx.callerId, ctx));
    if (actor) {
        unique_ptr<Action> action(new Action(ActionType::PauseConversation));
        actor->actionQueue().add(move(action));
    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable Routines::actionResumeConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> actor(getObjectById(ctx.callerId, ctx));
    if (actor) {
        unique_ptr<Action> action(new Action(ActionType::ResumeConversation));
        actor->actionQueue().add(move(action));
    } else {
        warn("Routine: creature not found: " + to_string(ctx.callerId));
    }

    return Variable();
}

Variable Routines::actionOpenDoor(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    shared_ptr<Object> actor(getObjectById(ctx.callerId, ctx));
    if (actor) {
        shared_ptr<Object> object(getObjectById(objectId, ctx));
        unique_ptr<ObjectAction> action(new ObjectAction(ActionType::OpenDoor, object));
        actor->actionQueue().add(move(action));
    } else {
        warn("Routine: object not found: " + to_string(objectId));
    }

    return Variable();
}

Variable Routines::actionCloseDoor(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    shared_ptr<Object> actor(getObjectById(ctx.callerId, ctx));
    if (actor) {
        shared_ptr<Object> object(getObjectById(objectId, ctx));
        unique_ptr<ObjectAction> action(new ObjectAction(ActionType::CloseDoor, object));
        actor->actionQueue().add(move(action));
    } else {
        warn("Routine: object not found: " + to_string(objectId));
    }

    return Variable();
}

Variable Routines::clearAllActions(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> actor(getObjectById(ctx.callerId, ctx));
    actor->clearAllActions();
    return Variable();
}

} // namespace game

} // namespace reone
