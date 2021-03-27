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

/** @file
 *  Implementation of routines related to object actions.
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
    float seconds = getFloat(args, 0);
    auto action = getAction(args, 1);

    auto objectAction = make_unique<CommandAction>(move(action));
    getCaller(ctx)->actionQueue().delay(move(objectAction), seconds);

    return Variable();
}

Variable Routines::assignCommand(const VariablesList &args, ExecutionContext &ctx) {
    auto subject = getObject(args, 0);
    auto action = getAction(args, 1);

    if (subject) {
        auto objectAction = make_unique<CommandAction>(move(action));
        subject->actionQueue().add(move(objectAction));
    } else {
        debug("Script: assignCommand: subject is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionDoCommand(const VariablesList &args, ExecutionContext &ctx) {
    auto action = getAction(args, 0);

    auto objectAction = make_unique<CommandAction>(move(action));
    getCaller(ctx)->actionQueue().add(move(objectAction));

    return Variable();
}

Variable Routines::actionMoveToObject(const VariablesList &args, ExecutionContext &ctx) {
    auto moveTo = getObject(args, 0);
    bool run = getBool(args, 1, false);
    float range = getFloat(args, 2, 1.0f);

    if (moveTo) {
        auto action = make_unique<MoveToObjectAction>(move(moveTo), run, range);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionMoveToObject: moveTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionStartConversation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: figure out all arguments
    auto objectToConverse = getObject(args, 0);
    string dialogResRef(getString(args, 1, ""));
    bool ignoreStartRange = getBool(args, 4, false);

    if (objectToConverse) {
        auto caller = getCaller(ctx);
        if (dialogResRef.empty()) {
            dialogResRef = caller->conversation();
        }
        auto action = make_unique<StartConversationAction>(move(objectToConverse), move(dialogResRef), ignoreStartRange);
        caller->actionQueue().add(move(action));
    } else {
        debug("Script: actionStartConversation: objectToConverse is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionPauseConversation(const VariablesList &args, ExecutionContext &ctx) {
    auto action = make_unique<Action>(ActionType::PauseConversation);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionResumeConversation(const VariablesList &args, ExecutionContext &ctx) {
    auto action = make_unique<Action>(ActionType::ResumeConversation);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionOpenDoor(const VariablesList &args, ExecutionContext &ctx) {
    auto door = getObject(args, 0);
    if (door) {
        auto action = make_unique<ObjectAction>(ActionType::OpenDoor, door);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionOpenDoor: door is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionCloseDoor(const VariablesList &args, ExecutionContext &ctx) {
    auto door = getObject(args, 0);
    if (door) {
        auto action = make_unique<ObjectAction>(ActionType::CloseDoor, door);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionCloseDoor: door is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::clearAllActions(const VariablesList &args, ExecutionContext &ctx) {
    getCaller(ctx)->clearAllActions();
    return Variable();
}

Variable Routines::actionJumpToObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(args, 0);
    bool walkStraightLine = getBool(args, 1, true);

    if (jumpTo) {
        auto action = make_unique<ObjectAction>(ActionType::JumpToObject, move(jumpTo));
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionJumpToObject: jumpTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionJumpToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto location = getLocationEngineType(args, 0);
    if (location) {
        auto action = make_unique<LocationAction>(ActionType::JumpToLocation, move(location));
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionJumpToLocation: location is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionForceMoveToObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto moveTo = getObject(args, 0);
    bool run = getBool(args, 1, false);
    float range = getFloat(args, 2, 1.0f);
    float timeout = getFloat(args, 3, 30.0f);

    if (moveTo) {
        auto action = make_unique<MoveToObjectAction>(move(moveTo), run, range);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionForceMoveToObject: moveTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionForceMoveToLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto destination = getLocationEngineType(args, 0);
    bool run = getBool(args, 1, false);
    float timeout = getFloat(args, 2, 30.0f);

    if (destination) {
        auto action = make_unique<MoveToLocationAction>(move(destination));
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionForceMoveToLocation: destination is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::jumpToObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(args, 0);
    bool walkStraightLine = getBool(args, 1, true);

    if (jumpTo) {
        auto action = make_unique<ObjectAction>(ActionType::JumpToObject, move(jumpTo));
        getCaller(ctx)->actionQueue().addToTop(move(action));
    } else {
        debug("Script: jumpToObject: jumpTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::jumpToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto destination = getLocationEngineType(args, 0);
    if (destination) {
        auto action = make_unique<LocationAction>(ActionType::JumpToLocation, move(destination));
        getCaller(ctx)->actionQueue().addToTop(move(action));
    } else {
        debug("Script: jumpToLocation: destination is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionRandomWalk(const VariablesList &args, ExecutionContext &ctx) {
    auto action = make_unique<Action>(ActionType::RandomWalk);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionMoveToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto destination = getLocationEngineType(args, 0);
    bool run = getBool(args, 1, false);

    if (destination) {
        auto action = make_unique<MoveToLocationAction>(move(destination), run);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionMoveToLocation: destination is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionMoveAwayFromObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto fleeFrom = getObject(args, 0);
    bool run = getBool(args, 1, false);
    float range = getFloat(args, 2, 40.0f);

    if (fleeFrom) {
        auto action = make_unique<Action>(ActionType::MoveAwayFromObject);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionMoveAwayFromObject: fleeFrom is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionEquipItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto item = getItem(args, 0);
    int inventorySlot = getInt(args, 1);
    bool instant = getBool(args, 2, false);

    if (item) {
        auto action = make_unique<Action>(ActionType::EquipItem);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionEquipItem: item is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionUnequipItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto item = getItem(args, 0);
    bool instant = getBool(args, 1, false);

    if (item) {
        auto action = make_unique<Action>(ActionType::UnequipItem);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionUnequipItem: item is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionPickUpItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto item = getItem(args, 0);
    if (item) {
        auto action = make_unique<Action>(ActionType::PickUpItem);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionPickUpItem: item is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionPutDownItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto item = getItem(args, 0);
    if (item) {
        auto action = make_unique<Action>(ActionType::DropItem);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionPutDownItem: item is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionAttack(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto caller = getCallerAsCreature(ctx);
    auto attackee = getSpatialObject(args, 0);
    bool passive = getBool(args, 1, false);

    if (caller && attackee) {
        auto action = make_unique<AttackAction>(attackee, caller->getAttackRange());
        caller->actionQueue().add(move(action));
    } else if (!caller) {
        debug("Script: actionAttack: caller is invalid", 1, DebugChannels::script);
    } else if (!attackee) {
        debug("Script: actionAttack: attackee is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionSpeakString(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    string toSpeak(getString(args, 0));
    auto talkVolume = getEnum(args, 1, TalkVolume::Talk);

    auto action = make_unique<Action>(ActionType::SpeakString);
    getCaller(ctx)->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionPlayAnimation(const VariablesList &args, ExecutionContext &ctx) {
    auto animation = getEnum<AnimationType>(args, 0);
    float speed = getFloat(args, 1, 1.0f);
    float duration = getFloat(args, 2, 0.0f);

    auto action = make_unique<PlayAnimationAction>(animation, speed, duration);
    getCaller(ctx)->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionCastSpellAtObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto spell = getEnum<ForcePower>(args, 0);
    auto target = getObject(args, 1);
    int metaMagic = getInt(args, 2, 0);
    bool cheat = getBool(args, 3, false);
    int domainLevel = getInt(args, 4, 0);
    auto projectilePathType = getEnum(args, 5, ProjectilePathType::Default);
    bool instantSpell = getBool(args, 6, false);

    auto action = make_unique<Action>(ActionType::CastSpellAtObject);
    getCaller(ctx)->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionGiveItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto item = getItem(args, 0);
    auto giveTo = getObject(args, 1);

    if (item && giveTo) {
        auto action = make_unique<Action>(ActionType::GiveItem);
        getCaller(ctx)->actionQueue().add(move(action));
    } else if (!item) {
        debug("Script: actionGiveItem: item is invalid", 1, DebugChannels::script);
    } else if (!giveTo) {
        debug("Script: actionGiveItem: giveTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionTakeItem(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto item = getItem(args, 0);
    auto takeFrom = getObject(args, 1);

    if (item && takeFrom) {
        auto action = make_unique<Action>(ActionType::TakeItem);
        getCaller(ctx)->actionQueue().add(move(action));
    } else if (!item) {
        debug("Script: actionTakeItem: item is invalid", 1, DebugChannels::script);
    } else if (!takeFrom) {
        debug("Script: actionTakeItem: takeFrom is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionForceFollowObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto follow = getObject(args, 0);
    float followDistance = getFloat(args, 1, 0.0f);

    if (follow) {
        auto action = make_unique<Action>(ActionType::ForceFollowObject);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionForceFollowObject: follow is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionWait(const VariablesList &args, ExecutionContext &ctx) {
    float seconds = getFloat(args, 0);

    auto action = make_unique<WaitAction>(seconds);
    getCaller(ctx)->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionCastSpellAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto spell = getEnum<ForcePower>(args, 0);
    auto targetLocation = getLocationEngineType(args, 1);
    int metaMagic = getInt(args, 2, 0);
    bool cheat = getBool(args, 3, false);
    auto projectilePathType = getEnum(args, 4, ProjectilePathType::Default);
    bool instantSpell = getBool(args, 5, false);

    if (targetLocation) {
        auto action = make_unique<Action>(ActionType::CastSpellAtLocation);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionCastSpellAtLocation: targetLocation is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionSpeakStringByStrRef(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    int strRef = getInt(args, 0);
    auto talkVolume = getEnum(args, 1, TalkVolume::Talk);

    auto action = make_unique<Action>(ActionType::SpeakStringByStrRef);
    getCaller(ctx)->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionUseFeat(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto feat = getEnum<Feat>(args, 0);
    auto target = getObject(args, 1);

    if (target) {
        auto action = make_unique<Action>(ActionType::UseFeat);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionUseFeat: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionUseSkill(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto skill = getEnum<Skill>(args, 0);
    auto target = getObject(args, 1);
    int subSkill = getInt(args, 2, 0);
    auto itemUsed = getObject(args, 3);

    if (target) {
        auto action = make_unique<Action>(ActionType::UseSkill);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionUseSkill: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionUseTalentOnObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto chosenTalen = getTalent(args, 0);
    auto target = getObject(args, 1);

    if (target) {
        auto action = make_unique<Action>(ActionType::UseTalentOnObject);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionUseTalentOnObject: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionUseTalentAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto chosenTalen = getTalent(args, 0);
    auto targetLocation = getLocationEngineType(args, 1);

    if (targetLocation) {
        auto action = make_unique<Action>(ActionType::UseTalentAtLocation);
        getCaller(ctx)->actionQueue().add(move(action));
    } else {
        debug("Script: actionUseTalentAtLocation: targetLocation is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionInteractObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::InteractObject);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionMoveAwayFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::MoveAwayFromLocation);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionSurrenderToEnemies(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::SurrenderToEnemies);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionEquipMostDamagingMelee(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::EquipMostDamagingMelee);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionEquipMostDamagingRanged(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::EquipMostDamagingRanged);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionEquipMostEffectiveArmor(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::EquipMostEffectiveArmor);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionUnlockObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::UnlockObject);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionLockObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::LockObject);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionCastFakeSpellAtObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::CastFakeSpellAtObject);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionCastFakeSpellAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: extract and pass all arguments to an action
    auto action = make_unique<Action>(ActionType::CastFakeSpellAtLocation);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionBarkString(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    int strRef = getInt(args, 0);

    auto action = make_unique<Action>(ActionType::BarkString);
    getCaller(ctx)->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionFollowLeader(const VariablesList &args, ExecutionContext &ctx) {
    auto action = make_unique<Action>(ActionType::FollowLeader);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::actionFollowOwner(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    float range = getFloat(args, 0, 2.5f);

    auto action = make_unique<Action>(ActionType::FollowOwner);
    getCaller(ctx)->actionQueue().add(move(action));

    return Variable();
}

Variable Routines::actionSwitchWeapons(const VariablesList &args, ExecutionContext &ctx) {
    auto action = make_unique<Action>(ActionType::SwitchWeapons);
    getCaller(ctx)->actionQueue().add(move(action));
    return Variable();
}

Variable Routines::getCurrentAction(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    shared_ptr<Action> action(object->actionQueue().getCurrentAction());
    return Variable::ofInt(static_cast<int>(action ? action->type() : ActionType::QueueEmpty));
}

} // namespace game

} // namespace reone
