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
 *  Implementation of action factory routines.
 */

#include "reone/game/script/routine/declarations.h"

#include "reone/system/logutil.h"

#include "reone/game/action/factory.h"
#include "reone/game/game.h"
#include "reone/game/object/creature.h"

#include "reone/game/script/routine/argutil.h"
#include "reone/game/script/routine/context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable actionRandomWalk(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newRandomWalk();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationArgument(args, 0);
    bool run = getIntAsBoolOrElse(args, 1, false);

    auto action = ctx.game.actionFactory().newMoveToLocation(move(destination), run);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto moveTo = getObject(args, 0, ctx);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);

    auto action = ctx.game.actionFactory().newMoveToObject(move(moveTo), run, range);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionMoveAwayFromObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto fleeFrom = getObject(args, 0, ctx);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 40.0f);

    auto action = ctx.game.actionFactory().newMoveAwayFromObject(move(fleeFrom), run, range);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    int inventorySlot = getInt(args, 1);
    bool instant = getIntAsBoolOrElse(args, 2, false);

    auto action = ctx.game.actionFactory().newEquipItem(move(item), inventorySlot, instant);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUnequipItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    bool instant = getIntAsBoolOrElse(args, 1, false);

    auto action = ctx.game.actionFactory().newUnequipItem(move(item), instant);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionAttack(const vector<Variable> &args, const RoutineContext &ctx) {
    auto attackee = getObject(args, 0, ctx);
    bool passive = getIntAsBoolOrElse(args, 1, false);

    auto caller = getCallerAsCreature(ctx);
    auto action = ctx.game.actionFactory().newAttack(attackee, caller->getAttackRange(), false, passive);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPlayAnimation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto animation = getIntAsEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float durationSeconds = getFloatOrElse(args, 2, 0.0f);

    auto action = ctx.game.actionFactory().newPlayAnimation(animation, speed, durationSeconds);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionOpenDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObjectAsDoor(args, 0, ctx);

    auto action = ctx.game.actionFactory().newOpenDoor(move(door));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCloseDoor(const vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObjectAsDoor(args, 0, ctx);

    auto action = ctx.game.actionFactory().newCloseDoor(move(door));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastSpellAtObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto target = getObject(args, 1, ctx);
    int metaMagic = getIntOrElse(args, 2, 0);
    bool cheat = getIntAsBoolOrElse(args, 3, false);
    int domainLevel = getIntOrElse(args, 4, 0);
    auto projectilePathType = getIntAsEnumOrElse(args, 5, ProjectilePathType::Default);
    bool instantSpell = getIntAsBoolOrElse(args, 6, false);

    auto action = ctx.game.actionFactory().newCastSpellAtObject(spell, move(target), metaMagic, cheat, domainLevel, projectilePathType, instantSpell);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionGiveItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto giveTo = getObject(args, 1, ctx);

    auto action = ctx.game.actionFactory().newGiveItem(move(item), move(giveTo));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionTakeItem(const vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto takeFrom = getObject(args, 1, ctx);

    auto action = ctx.game.actionFactory().newTakeItem(move(item), move(takeFrom));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionJumpToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto jumpTo = getObject(args, 0, ctx);
    bool walkStraightLine = getIntAsBoolOrElse(args, 1, true);

    auto action = ctx.game.actionFactory().newJumpToObject(move(jumpTo), walkStraightLine);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionWait(const vector<Variable> &args, const RoutineContext &ctx) {
    float seconds = getFloat(args, 0);

    auto action = ctx.game.actionFactory().newWait(seconds);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionStartConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto objectToConverse = getObject(args, 0, ctx);
    string dialogResRef = getStringOrElse(args, 1, "");
    bool privateConversation = getIntAsBoolOrElse(args, 2, false);
    auto conversationType = getIntAsEnumOrElse(args, 3, ConversationType::Cinematic);
    bool ignoreStartRange = getIntAsBoolOrElse(args, 4, false);
    string nameObjectToIgnore1 = getStringOrElse(args, 5, "");
    string nameObjectToIgnore2 = getStringOrElse(args, 6, "");
    string nameObjectToIgnore3 = getStringOrElse(args, 7, "");
    string nameObjectToIgnore4 = getStringOrElse(args, 8, "");
    string nameObjectToIgnore5 = getStringOrElse(args, 9, "");
    string nameObjectToIgnore6 = getStringOrElse(args, 10, "");
    bool useLeader = getIntAsBoolOrElse(args, 11, false);
    int barkX = getIntOrElse(args, 12, -1);
    int barkY = getIntOrElse(args, 13, -1);
    bool dontClearAllActions = getIntAsBoolOrElse(args, 14, false);

    auto caller = getCaller(ctx);
    if (dialogResRef.empty()) {
        dialogResRef = caller->conversation();
    }
    auto action = ctx.game.actionFactory().newStartConversation(
        move(objectToConverse),
        move(dialogResRef),
        privateConversation,
        conversationType,
        ignoreStartRange,
        nameObjectToIgnore1,
        nameObjectToIgnore2,
        nameObjectToIgnore3,
        nameObjectToIgnore4,
        nameObjectToIgnore5,
        nameObjectToIgnore6,
        useLeader,
        barkX,
        barkY,
        dontClearAllActions);
    caller->addAction(move(action));

    return Variable::ofNull();
}

Variable actionPauseConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newPauseConversation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionResumeConversation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newResumeConversation();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionJumpToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationArgument(args, 0);

    auto action = ctx.game.actionFactory().newJumpToLocation(move(location));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUseSkill(const vector<Variable> &args, const RoutineContext &ctx) {
    auto skill = getIntAsEnum<SkillType>(args, 0);
    auto target = getObject(args, 1, ctx);
    int subSkill = getIntOrElse(args, 2, 0);
    auto itemUsed = getObjectAsItemOrNull(args, 3, ctx);

    auto action = ctx.game.actionFactory().newUseSkill(move(target), skill, subSkill, move(itemUsed));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionDoCommand(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = getAction(args, 0);

    auto commandAction = ctx.game.actionFactory().newDoCommand(move(action));
    getCaller(ctx)->addAction(move(commandAction));

    return Variable::ofNull();
}

Variable actionUseTalentOnObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto chosenTalent = getTalent(args, 0);
    auto target = getObject(args, 1, ctx);

    auto action = ctx.game.actionFactory().newUseTalentOnObject(move(chosenTalent), move(target));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionInteractObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto placeable = getObjectAsPlaceable(args, 0, ctx);

    auto action = ctx.game.actionFactory().newInteractObject(move(placeable));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSurrenderToEnemies(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newSurrenderToEnemies();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationArgument(args, 0);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float timeout = getFloatOrElse(args, 2, 30.0f);

    auto action = ctx.game.actionFactory().newMoveToLocation(move(destination), run, timeout, true);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto moveTo = getObject(args, 0, ctx);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);
    float timeout = getFloatOrElse(args, 3, 30.0f);

    auto action = ctx.game.actionFactory().newMoveToObject(move(moveTo), run, range, timeout, true);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingMelee(const vector<Variable> &args, const RoutineContext &ctx) {
    auto versus = getObjectOrNull(args, 0, ctx);
    bool offHand = getIntAsBoolOrElse(args, 1, false);

    auto action = ctx.game.actionFactory().newEquipMostDamagingMelee(move(versus), offHand);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingRanged(const vector<Variable> &args, const RoutineContext &ctx) {
    auto versus = getObjectOrNull(args, 0, ctx);

    auto action = ctx.game.actionFactory().newEquipMostDamagingRanged(move(versus));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionUnlockObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);

    auto action = ctx.game.actionFactory().newUnlockObject(move(target));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionLockObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);

    auto action = ctx.game.actionFactory().newLockObject(move(target));
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtObject(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto target = getObject(args, 1, ctx);
    auto projectilePathType = getIntAsEnum<ProjectilePathType>(args, 2);

    auto action = ctx.game.actionFactory().newCastFakeSpellAtObject(spell, move(target), projectilePathType);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtLocation(const vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto location = getLocationArgument(args, 1);
    auto projectilePathType = getIntAsEnum<ProjectilePathType>(args, 2);

    auto action = ctx.game.actionFactory().newCastFakeSpellAtLocation(spell, move(location), projectilePathType);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionBarkString(const vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);

    auto action = ctx.game.actionFactory().newBarkString(strRef);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionFollowLeader(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newFollowLeader();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

// TSL

Variable actionFollowOwner(const vector<Variable> &args, const RoutineContext &ctx) {
    float range = getFloatOrElse(args, 0, 2.5f);

    auto action = ctx.game.actionFactory().newFollowOwner(range);
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

Variable actionSwitchWeapons(const vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newSwitchWeapons();
    getCaller(ctx)->addAction(move(action));

    return Variable::ofNull();
}

// END TSL

} // namespace routine

} // namespace game

} // namespace reone
