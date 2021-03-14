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

#include "routines.h"

#include "../../common/log.h"

#include "../action/commandaction.h"
#include "../action/locationaction.h"
#include "../action/movetolocation.h"
#include "../action/movetoobject.h"
#include "../action/playanimation.h"
#include "../action/startconversation.h"
#include "../action/waitaction.h"
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

Variable Routines::actionRandomWalk(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::RandomWalk);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionRandomWalk: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionMoveToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto destination = getLocationEngineType(args, 0);
        bool run = getBool(args, 1, false);
        auto action = make_unique<MoveToLocationAction>(destination, run);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionMoveToLocation: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionMoveAwayFromObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::MoveAwayFromObject);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionMoveAwayFromObject: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionEquipItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::EquipItem);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionEquipItem: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionUnequipItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::UnequipItem);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionUnequipItem: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionPickUpItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::PickUpItem);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionPickUpItem: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionPutDownItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::DropItem);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionPutDownItem: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionAttack(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    if (!caller) {
        warn("Routines: actionAttack: caller is invalid");
        return Variable();
    }
    auto attackee = getSpatialObject(args, 0);
    if (!attackee) {
        warn("Routines: actionAttack: attackee is invalid");
        return Variable();
    }
    auto action = make_unique<AttackAction>(attackee);
    caller->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionSpeakString(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::SpeakString);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionSpeakString: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionPlayAnimation(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        AnimationType animation = static_cast<AnimationType>(getInt(args, 0));
        float speed = getFloat(args, 1, 1.0f);
        float duration = getFloat(args, 2, 0.0f);

        auto action = make_unique<PlayAnimationAction>(animation, speed, duration);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionPlayAnimation: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionCastSpellAtObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::CastSpellAtObject);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionCastSpellAtObject: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionGiveItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::GiveItem);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionGiveItem: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionTakeItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::TakeItem);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionTakeItem: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionForceFollowObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::ForceFollowObject);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionForceFollowObject: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionWait(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCaller(ctx);
    if (caller) {
        float seconds = getFloat(args, 0);
        auto action = make_unique<WaitAction>(seconds);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionWait: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionCastSpellAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::CastSpellAtLocation);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionCastSpellAtLocation: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionSpeakStringByStrRef(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::SpeakStringByStrRef);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionSpeakStringByStrRef: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionUseFeat(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::UseFeat);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionUseFeat: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionUseSkill(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::UseSkill);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionUseSkill: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionUseTalentOnObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::UseTalentOnObject);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionUseSkill: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionUseTalentAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::UseTalentAtLocation);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionUseTalentAtLocation: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionInteractObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::InteractObject);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionInteractObject: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionMoveAwayFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::MoveAwayFromLocation);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionMoveAwayFromLocation: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionSurrenderToEnemies(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::SurrenderToEnemies);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionSurrenderToEnemies: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionEquipMostDamagingMelee(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::EquipMostDamagingMelee);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionEquipMostDamagingMelee: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionEquipMostDamagingRanged(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::EquipMostDamagingRanged);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionEquipMostDamagingRanged: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionEquipMostEffectiveArmor(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::EquipMostEffectiveArmor);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionEquipMostEffectiveArmor: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionUnlockObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::UnlockObject);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionUnlockObject: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionLockObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::LockObject);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionLockObject: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionCastFakeSpellAtObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::CastFakeSpellAtObject);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionCastFakeSpellAtObject: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionCastFakeSpellAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::CastFakeSpellAtLocation);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionCastFakeSpellAtLocation: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionBarkString(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::BarkString);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionBarkString: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionFollowLeader(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::FollowLeader);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionFollowLeader: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionFollowOwner(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::FollowOwner);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionFollowOwner: caller is invalid");
    }
    return Variable();
}

Variable Routines::actionSwitchWeapons(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: handle arguments
    auto caller = getCallerAsCreature(ctx);
    if (caller) {
        auto action = make_unique<Action>(ActionType::SwitchWeapons);
        caller->actionQueue().add(move(action));
    } else {
        warn("Routines: actionSwitchWeapons: caller is invalid");
    }
    return Variable();
}

} // namespace game

} // namespace reone
