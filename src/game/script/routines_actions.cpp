/*
 * Copyright (c) 2020 The reone project contributors
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
#include "../enginetype/location.h"
#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::delayCommand(const VariablesList &args, ExecutionContext &ctx) {
    float delay = getFloat(args, 0);
    auto action = make_unique<CommandAction>(getAction(args, 1));

    auto subject = getCaller(ctx);
    subject->actionQueue().delay(move(action), delay);

    return Variable();
}

Variable Routines::assignCommand(const VariablesList &args, ExecutionContext &ctx) {
    auto subject = getObject(args, 0);
    if (subject) {
        auto action = make_unique<CommandAction>(getAction(args, 1));
        subject->actionQueue().add(move(action));
    } else {
        warn("Routines: assignCommand: subject is invalid");
    }
    return Variable();
}

Variable Routines::actionDoCommand(const VariablesList &args, ExecutionContext &ctx) {
    auto action = make_unique<CommandAction>(getAction(args, 0));

    auto subject = getCaller(ctx);
    subject->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionMoveToObject(const VariablesList &args, ExecutionContext &ctx) {
    auto moveTo = getObject(args, 0);
    if (moveTo) {
        bool run = getBool(args, 1, false);
        float range = getFloat(args, 2, 1.0f);
        auto action = make_unique<MoveToObjectAction>(moveTo, run, range);

        auto subject = getCaller(ctx);
        subject->actionQueue().add(move(action));

    } else {
        warn("Routines: actionMoveToObject: moveTo is invalid");
    }

    return Variable();
}

Variable Routines::actionStartConversation(const VariablesList &args, ExecutionContext &ctx) {
    auto objectToConverse = getObject(args, 0);
    if (objectToConverse) {
        auto subject = getCaller(ctx);

        string dialogResRef = getString(args, 1, "");
        if (dialogResRef.empty()) {
            dialogResRef = subject->conversation();
        }

        bool ignoreStartRange = getBool(args, 4, false);
        auto action = make_unique<StartConversationAction>(objectToConverse, dialogResRef, ignoreStartRange);
        subject->actionQueue().add(move(action));

    } else {
        warn("Routines: actionStartConversation: objectToConverse is invalid");
    }

    return Variable();
}

Variable Routines::actionPauseConversation(const VariablesList &args, ExecutionContext &ctx) {
    auto action = make_unique<Action>(ActionType::PauseConversation);

    auto subject = getCaller(ctx);
    subject->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionResumeConversation(const VariablesList &args, ExecutionContext &ctx) {
    auto action = make_unique<Action>(ActionType::ResumeConversation);

    auto subject = getCaller(ctx);
    subject->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionOpenDoor(const VariablesList &args, ExecutionContext &ctx) {
    auto door = getObject(args, 0);
    if (door) {
        auto action = make_unique<ObjectAction>(ActionType::OpenDoor, door);
        auto subject = getCaller(ctx);
        subject->actionQueue().add(move(action));
    } else {
        warn("Routines: actionOpenDoor: door is invalid");
    }

    return Variable();
}

Variable Routines::actionCloseDoor(const VariablesList &args, ExecutionContext &ctx) {
    auto door = getObject(args, 0);
    if (door) {
        auto action = make_unique<ObjectAction>(ActionType::CloseDoor, door);
        auto subject = getCaller(ctx);
        subject->actionQueue().add(move(action));
    } else {
        warn("Routines: actionCloseDoor: door is invalid");
    }

    return Variable();
}

Variable Routines::clearAllActions(const VariablesList &args, ExecutionContext &ctx) {
    auto subject = getCaller(ctx);
    subject->clearAllActions();
    return Variable();
}

Variable Routines::actionJumpToObject(const VariablesList &args, ExecutionContext &ctx) {
    auto toJumpTo = getObject(args, 0);
    if (toJumpTo) {
        auto action = make_unique<ObjectAction>(ActionType::JumpToObject, toJumpTo);
        auto subject = getCaller(ctx);
        subject->actionQueue().add(move(action));
    } else {
        warn("Routines: actionJumpToObject: toJumpTo is invalid");
    }

    return Variable();
}

Variable Routines::actionJumpToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto location = getLocationEngineType(args, 0);
    if (location) {
        auto action = make_unique<LocationAction>(ActionType::JumpToLocation, location);
        auto subject = getCaller(ctx);
        subject->actionQueue().add(move(action));
    } else {
        warn("Routines: actionJumpToLocation: location is invalid");
    }

    return Variable();
}

Variable Routines::actionForceMoveToObject(const VariablesList &args, ExecutionContext &ctx) {
    auto moveTo = getObject(args, 0);
    if (moveTo) {
        bool run = getInt(args, 1, false);
        float range = getFloat(args, 2, 1.0f);
        auto action = make_unique<MoveToObjectAction>(moveTo, run, range);

        auto subject = getCaller(ctx);
        subject->actionQueue().add(move(action));

    } else {
        warn("Routines: actionForceMoveToObject: moveTo is invalid");
    }

    return Variable();
}

Variable Routines::actionForceMoveToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto destination = getLocationEngineType(args, 0);
    if (destination) {
        auto action = make_unique<MoveToPointAction>(destination->position());
        auto subject = getCaller(ctx);
        subject->actionQueue().add(move(action));
    } else {
        warn("Routines: actionForceMoveToLocation: destination is invalid");
    }

    return Variable();
}

Variable Routines::jumpToObject(const VariablesList &args, ExecutionContext &ctx) {
    auto toJumpTo = getObject(args, 0);
    if (toJumpTo) {
        auto action = make_unique<ObjectAction>(ActionType::JumpToObject, toJumpTo);

        auto subject = static_pointer_cast<Object>(ctx.caller);
        subject->actionQueue().clear();
        subject->actionQueue().add(move(action));

    } else {
        warn("Routines: jumpToObject: toJumpTo is invalid");
    }

    return Variable();
}

Variable Routines::jumpToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto location = getLocationEngineType(args, 0);
    if (location) {
        auto action = make_unique<LocationAction>(ActionType::JumpToLocation, location);

        auto subject = getCaller(ctx);
        subject->actionQueue().clear();
        subject->actionQueue().add(move(action));

    } else {
        warn("Routines: jumpToLocation: location is invalid");
    }

    return Variable();
}

} // namespace game

} // namespace reone
