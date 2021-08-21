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
 *  Implementation of action-related routines.
 */

#include "../../routines.h"

#include "../../../../common/log.h"

#include "../../../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::actionRandomWalk(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionMoveToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto destination = getLocationEngineType(args, 0);
    bool run = getBool(args, 1, false);

    if (destination) {
        auto action = _game.services().actionFactory().newMoveToLocation(move(destination), run);
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionMoveToLocation: destination is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionMoveToObject(const VariablesList &args, ExecutionContext &ctx) {
    auto moveTo = getObject(args, 0, ctx);
    bool run = getBool(args, 1, false);
    float range = getFloat(args, 2, 1.0f);

    if (moveTo) {
        auto action = _game.services().actionFactory().newMoveToObject(move(moveTo), run, range);
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionMoveToObject: moveTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionMoveAwayFromObject(const VariablesList &args, ExecutionContext &ctx) {
    auto fleeFrom = getObject(args, 0, ctx);
    bool run = getBool(args, 1, false);
    float range = getFloat(args, 2, 40.0f);

    if (fleeFrom) {
        // TODO: add action to caller
    } else {
        debug("Script: actionMoveAwayFromObject: fleeFrom is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionEquipItem(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0, ctx);
    int inventorySlot = getInt(args, 1);
    bool instant = getBool(args, 2, false);

    if (item) {
        // TODO: add action to caller
    } else {
        debug("Script: actionEquipItem: item is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionUnequipItem(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0, ctx);
    bool instant = getBool(args, 1, false);

    if (item) {
        // TODO: add action to caller
    } else {
        debug("Script: actionUnequipItem: item is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionPickUpItem(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0, ctx);
    if (item) {
        // TODO: add action to caller
    } else {
        debug("Script: actionPickUpItem: item is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionPutDownItem(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0, ctx);
    if (item) {
        // TODO: add action to caller
    } else {
        debug("Script: actionPutDownItem: item is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionAttack(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto caller = getCallerAsCreature(ctx);
    auto attackee = getSpatialObject(args, 0, ctx);
    bool passive = getBool(args, 1, false);

    if (caller && attackee) {
        auto action = _game.services().actionFactory().newAttack(attackee, caller->getAttackRange());
        caller->addAction(move(action));
    } else if (!caller) {
        debug("Script: actionAttack: caller is invalid", 1, DebugChannels::script);
    } else if (!attackee) {
        debug("Script: actionAttack: attackee is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionSpeakString(const VariablesList &args, ExecutionContext &ctx) {
    string toSpeak(getString(args, 0));
    auto talkVolume = getEnum(args, 1, TalkVolume::Talk);
    
    // TODO: add action to caller

    return Variable();
}

Variable Routines::actionPlayAnimation(const VariablesList &args, ExecutionContext &ctx) {
    auto animation = getEnum<AnimationType>(args, 0);
    float speed = getFloat(args, 1, 1.0f);
    float duration = getFloat(args, 2, 0.0f);

    auto action = _game.services().actionFactory().newPlayAnimation(animation, speed, duration);
    getCaller(ctx)->addAction(move(action));

    return Variable();
}

Variable Routines::actionOpenDoor(const VariablesList &args, ExecutionContext &ctx) {
    auto door = getObject(args, 0, ctx);
    if (door) {
        auto action = _game.services().actionFactory().newOpenDoor(door);
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionOpenDoor: door is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionCloseDoor(const VariablesList &args, ExecutionContext &ctx) {
    auto door = getObject(args, 0, ctx);
    if (door) {
        auto action = _game.services().actionFactory().newCloseDoor(door);
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionCloseDoor: door is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionCastSpellAtObject(const VariablesList &args, ExecutionContext &ctx) {
    auto spell = getEnum<ForcePower>(args, 0);
    auto target = getObject(args, 1, ctx);
    int metaMagic = getInt(args, 2, 0);
    bool cheat = getBool(args, 3, false);
    int domainLevel = getInt(args, 4, 0);
    auto projectilePathType = getEnum(args, 5, ProjectilePathType::Default);
    bool instantSpell = getBool(args, 6, false);

    // TODO: add action to caller

    return Variable();
}

Variable Routines::actionGiveItem(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0, ctx);
    auto giveTo = getObject(args, 1, ctx);

    if (item && giveTo) {
        // TODO: add action to caller
    } else if (!item) {
        debug("Script: actionGiveItem: item is invalid", 1, DebugChannels::script);
    } else if (!giveTo) {
        debug("Script: actionGiveItem: giveTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionTakeItem(const VariablesList &args, ExecutionContext &ctx) {
    auto item = getItem(args, 0, ctx);
    auto takeFrom = getObject(args, 1, ctx);

    if (item && takeFrom) {
        // TODO: add action to caller
    } else if (!item) {
        debug("Script: actionTakeItem: item is invalid", 1, DebugChannels::script);
    } else if (!takeFrom) {
        debug("Script: actionTakeItem: takeFrom is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionForceFollowObject(const VariablesList &args, ExecutionContext &ctx) {
    auto follow = getObject(args, 0, ctx);
    float followDistance = getFloat(args, 1, 0.0f);

    if (follow) {
        // TODO: add action to caller
    } else {
        debug("Script: actionForceFollowObject: follow is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionJumpToObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(args, 0, ctx);
    bool walkStraightLine = getBool(args, 1, true);

    if (jumpTo) {
        auto action = _game.services().actionFactory().newJumpToObject(move(jumpTo));
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionJumpToObject: jumpTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionWait(const VariablesList &args, ExecutionContext &ctx) {
    float seconds = getFloat(args, 0);

    auto action = _game.services().actionFactory().newWait(seconds);
    getCaller(ctx)->addAction(move(action));

    return Variable();
}

Variable Routines::actionStartConversation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: figure out all arguments
    auto objectToConverse = getObject(args, 0, ctx);
    string dialogResRef(getString(args, 1, ""));
    bool ignoreStartRange = getBool(args, 4, false);

    if (objectToConverse) {
        auto caller = getCaller(ctx);
        if (dialogResRef.empty()) {
            dialogResRef = caller->conversation();
        }
        auto action = _game.services().actionFactory().newStartConversation(move(objectToConverse), move(dialogResRef), ignoreStartRange);
        caller->addAction(move(action));
    } else {
        debug("Script: actionStartConversation: objectToConverse is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionPauseConversation(const VariablesList &args, ExecutionContext &ctx) {
    auto action = _game.services().actionFactory().newPauseConversation();
    getCaller(ctx)->addAction(move(action));
    return Variable();
}

Variable Routines::actionResumeConversation(const VariablesList &args, ExecutionContext &ctx) {
    auto action = _game.services().actionFactory().newResumeConversation();
    getCaller(ctx)->addAction(move(action));
    return Variable();
}

Variable Routines::actionJumpToLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto location = getLocationEngineType(args, 0);
    if (location) {
        auto action = _game.services().actionFactory().newJumpToLocation(move(location));
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionJumpToLocation: location is invalid", 1, DebugChannels::script);
    }
    return Variable();
}

Variable Routines::actionCastSpellAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto spell = getEnum<ForcePower>(args, 0);
    auto targetLocation = getLocationEngineType(args, 1);
    int metaMagic = getInt(args, 2, 0);
    bool cheat = getBool(args, 3, false);
    auto projectilePathType = getEnum(args, 4, ProjectilePathType::Default);
    bool instantSpell = getBool(args, 5, false);

    if (targetLocation) {
        // TODO: add action to caller
    } else {
        debug("Script: actionCastSpellAtLocation: targetLocation is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionSpeakStringByStrRef(const VariablesList &args, ExecutionContext &ctx) {
    int strRef = getInt(args, 0);
    auto talkVolume = getEnum(args, 1, TalkVolume::Talk);

    // TODO: add action to caller

    return Variable();
}

Variable Routines::actionUseFeat(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto feat = getEnum<FeatType>(args, 0);
    auto target = getObject(args, 1, ctx);

    if (target) {
        auto action = _game.services().actionFactory().newUseFeat(target, feat);
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionUseFeat: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionUseSkill(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto skill = getEnum<SkillType>(args, 0);
    auto target = getObject(args, 1, ctx);
    int subSkill = getInt(args, 2, 0);
    auto itemUsed = getObject(args, 3, ctx);

    if (target) {
        auto action = _game.services().actionFactory().newUseSkill(target, skill);
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionUseSkill: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionDoCommand(const VariablesList &args, ExecutionContext &ctx) {
    auto action = getAction(args, 0);

    auto commandAction = _game.services().actionFactory().newDoCommand(move(action));
    getCaller(ctx)->addAction(move(commandAction));

    return Variable();
}

Variable Routines::actionUseTalentOnObject(const VariablesList &args, ExecutionContext &ctx) {
    auto chosenTalen = getTalent(args, 0);
    auto target = getObject(args, 1, ctx);

    if (target) {
        // TODO: add action to caller
    } else {
        debug("Script: actionUseTalentOnObject: target is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionUseTalentAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    auto chosenTalen = getTalent(args, 0);
    auto targetLocation = getLocationEngineType(args, 1);

    if (targetLocation) {
        // TODO: add action to caller
    } else {
        debug("Script: actionUseTalentAtLocation: targetLocation is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionInteractObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionMoveAwayFromLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionSurrenderToEnemies(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionForceMoveToLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto destination = getLocationEngineType(args, 0);
    bool run = getBool(args, 1, false);
    float timeout = getFloat(args, 2, 30.0f);

    if (destination) {
        auto action = _game.services().actionFactory().newMoveToLocation(move(destination));
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionForceMoveToLocation: destination is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionForceMoveToObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: pass all arguments to an action
    auto moveTo = getObject(args, 0, ctx);
    bool run = getBool(args, 1, false);
    float range = getFloat(args, 2, 1.0f);
    float timeout = getFloat(args, 3, 30.0f);

    if (moveTo) {
        auto action = _game.services().actionFactory().newMoveToObject(move(moveTo), run, range);
        getCaller(ctx)->addAction(move(action));
    } else {
        debug("Script: actionForceMoveToObject: moveTo is invalid", 1, DebugChannels::script);
    }

    return Variable();
}

Variable Routines::actionEquipMostDamagingMelee(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionEquipMostDamagingRanged(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionEquipMostEffectiveArmor(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionUnlockObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionLockObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionCastFakeSpellAtObject(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionCastFakeSpellAtLocation(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionBarkString(const VariablesList &args, ExecutionContext &ctx) {
    int strRef = getInt(args, 0);
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionFollowLeader(const VariablesList &args, ExecutionContext &ctx) {
    auto action = _game.services().actionFactory().newFollowLeader();
    getCaller(ctx)->addAction(move(action));
    return Variable();
}

Variable Routines::actionFollowOwner(const VariablesList &args, ExecutionContext &ctx) {
    float range = getFloat(args, 0, 2.5f);
    // TODO: add action to caller
    return Variable();
}

Variable Routines::actionSwitchWeapons(const VariablesList &args, ExecutionContext &ctx) {
    // TODO: add action to caller
    return Variable();
}

} // namespace game

} // namespace reone
