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

#include "declarations.h"

#include "../../../../../common/logutil.h"
#include "../../../../../script/exception/argument.h"

#include "../../../action/factory.h"
#include "../../../game.h"
#include "../../../object/creature.h"

#include "../context.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable actionRandomWalk(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.actionFactory.newRandomWalk();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationEngineType(args, 0);
    bool run = getBoolOrElse(args, 1, false);

    auto action = ctx.actionFactory.newMoveToLocation(move(destination), run);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto moveTo = getObject(args, 0, ctx);
    bool run = getBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);

    auto action = ctx.actionFactory.newMoveToObject(move(moveTo), run, range);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveAwayFromObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto fleeFrom = getObject(args, 0, ctx);
    bool run = getBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 40.0f);

    auto action = ctx.actionFactory.newMoveAwayFromObject(move(fleeFrom), run, range);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    int inventorySlot = getInt(args, 1);
    bool instant = getBoolOrElse(args, 2, false);

    auto action = ctx.actionFactory.newEquipItem(move(item), inventorySlot, instant);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUnequipItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    bool instant = getBoolOrElse(args, 1, false);

    auto action = ctx.actionFactory.newUnequipItem(move(item), instant);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPickUpItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);

    auto action = ctx.actionFactory.newPickUpItem(move(item));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPutDownItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);

    auto action = ctx.actionFactory.newPutDownItem(move(item));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionAttack(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    auto attackee = getSpatialObject(args, 0, ctx);
    bool passive = getBoolOrElse(args, 1, false);

    auto action = ctx.actionFactory.newAttack(attackee, caller->getAttackRange(), false, passive);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSpeakString(const vector<Variable> &args, const RoutineContext &ctx) {
    auto caller = getCallerAsCreature(ctx);
    string stringToSpeak(getString(args, 0));
    auto talkVolume = getEnumOrElse(args, 1, TalkVolume::Talk);

    auto action = ctx.actionFactory.newSpeakString(move(stringToSpeak), talkVolume);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPlayAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto animation = getEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float durationSeconds = getFloatOrElse(args, 2, 0.0f);

    auto action = ctx.actionFactory.newPlayAnimation(animation, speed, durationSeconds);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionOpenDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObject(args, 0, ctx);

    auto action = ctx.actionFactory.newOpenDoor(door);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCloseDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObject(args, 0, ctx);

    auto action = ctx.actionFactory.newCloseDoor(door);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastSpellAtObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getEnum<ForcePower>(args, 0);
    auto target = getObject(args, 1, ctx);
    int metaMagic = getIntOrElse(args, 2, 0);
    bool cheat = getBoolOrElse(args, 3, false);
    int domainLevel = getIntOrElse(args, 4, 0);
    auto projectilePathType = getEnumOrElse(args, 5, ProjectilePathType::Default);
    bool instantSpell = getBoolOrElse(args, 6, false);

    auto action = ctx.actionFactory.newCastSpellAtObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionGiveItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    auto giveTo = getObject(args, 1, ctx);

    auto action = ctx.actionFactory.newGiveItem();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionTakeItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getItem(args, 0, ctx);
    auto takeFrom = getObject(args, 1, ctx);

    auto action = ctx.actionFactory.newTakeItem();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceFollowObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto follow = getObject(args, 0, ctx);
    float followDistance = getFloatOrElse(args, 1, 0.0f);

    auto action = ctx.actionFactory.newForceFollowObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionJumpToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto jumpTo = getObject(args, 0, ctx);
    bool walkStraightLine = getBoolOrElse(args, 1, true);

    auto action = ctx.actionFactory.newJumpToObject(move(jumpTo));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionWait(const vector<Variable> &args, const RoutineContext &ctx) {
    float seconds = getFloat(args, 0);

    auto action = ctx.actionFactory.newWait(seconds);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionStartConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: figure out all arguments
    auto objectToConverse = getObject(args, 0, ctx);
    string dialogResRef(getStringOrElse(args, 1, ""));
    bool ignoreStartRange = getBoolOrElse(args, 4, false);

    auto caller = getCaller(ctx);
    if (dialogResRef.empty()) {
        dialogResRef = caller->conversation();
    }
    auto action = ctx.actionFactory.newStartConversation(move(objectToConverse), move(dialogResRef), ignoreStartRange);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPauseConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.actionFactory.newPauseConversation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionResumeConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.actionFactory.newResumeConversation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionJumpToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationEngineType(args, 0);

    auto action = ctx.actionFactory.newJumpToLocation(move(location));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastSpellAtLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getEnum<ForcePower>(args, 0);
    auto targetLocation = getLocationEngineType(args, 1);
    int metaMagic = getIntOrElse(args, 2, 0);
    bool cheat = getBoolOrElse(args, 3, false);
    auto projectilePathType = getEnumOrElse(args, 4, ProjectilePathType::Default);
    bool instantSpell = getBoolOrElse(args, 5, false);

    auto action = ctx.actionFactory.newCastSpellAtLocation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSpeakStringByStrRef(const vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);
    auto talkVolume = getEnumOrElse(args, 1, TalkVolume::Talk);

    auto action = ctx.actionFactory.newSpeakStringByStrRef();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUseFeat(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto feat = getEnum<FeatType>(args, 0);
    auto target = getObject(args, 1, ctx);

    auto action = ctx.actionFactory.newUseFeat(target, feat);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUseSkill(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto skill = getEnum<SkillType>(args, 0);
    auto target = getObject(args, 1, ctx);
    int subSkill = getIntOrElse(args, 2, 0);
    auto itemUsed = getObject(args, 3, ctx);

    auto action = ctx.actionFactory.newUseSkill(target, skill);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionDoCommand(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = getAction(args, 0);

    auto commandAction = ctx.actionFactory.newDoCommand(move(action));
    getCaller(ctx)->addAction(move(commandAction));

    return Variable::ofNull();
}

Variable actionUseTalentOnObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto chosenTalen = getTalent(args, 0);
    auto target = getObject(args, 1, ctx);

    auto action = ctx.actionFactory.newUseTalentOnObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUseTalentAtLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto chosenTalen = getTalent(args, 0);
    auto targetLocation = getLocationEngineType(args, 1);

    auto action = ctx.actionFactory.newUseTalentAtLocation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionInteractObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newInteractObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveAwayFromLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newMoveAwayFromLocation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSurrenderToEnemies(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newSurrenderToEnemies();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto destination = getLocationEngineType(args, 0);
    bool run = getBoolOrElse(args, 1, false);
    float timeout = getFloatOrElse(args, 2, 30.0f);

    auto action = ctx.actionFactory.newMoveToLocation(move(destination));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: pass all arguments to an action
    auto moveTo = getObject(args, 0, ctx);
    bool run = getBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);
    float timeout = getFloatOrElse(args, 3, 30.0f);

    auto action = ctx.actionFactory.newMoveToObject(move(moveTo), run, range);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingMelee(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newEquipMostDamagingMelee();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingRanged(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newEquipMostDamagingRanged();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostEffectiveArmor(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newEquipMostEffectiveArmor();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUnlockObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newUnlockObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionLockObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newLockObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtObject(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newCastFakeSpellAtObject();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newCastFakeSpellAtLocation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionBarkString(const vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);

    auto action = ctx.actionFactory.newBarkString();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionFollowLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.actionFactory.newFollowLeader();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionFollowOwner(const vector<Variable> &args, const RoutineContext &ctx) {
    float range = getFloatOrElse(args, 0, 2.5f);

    auto action = ctx.actionFactory.newFollowOwner();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSwitchWeapons(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: arguments

    auto action = ctx.actionFactory.newSwitchWeapons();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

} // namespace routine

} // namespace game

} // namespace reone
