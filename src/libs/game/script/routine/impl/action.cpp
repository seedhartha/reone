/*
 * Copyright (c) 2020-2023 The reone project contributors
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

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable actionRandomWalk(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newRandomWalk();
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionMoveToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationArgument(args, 0);
    bool run = getIntAsBoolOrElse(args, 1, false);

    auto action = ctx.game.actionFactory().newMoveToLocation(std::move(destination), run);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionMoveToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto moveTo = getObject(args, 0, ctx);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);

    auto action = ctx.game.actionFactory().newMoveToObject(std::move(moveTo), run, range);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionMoveAwayFromObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto fleeFrom = getObject(args, 0, ctx);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 40.0f);

    auto action = ctx.game.actionFactory().newMoveAwayFromObject(std::move(fleeFrom), run, range);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionEquipItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    int inventorySlot = getInt(args, 1);
    bool instant = getIntAsBoolOrElse(args, 2, false);

    auto action = ctx.game.actionFactory().newEquipItem(std::move(item), inventorySlot, instant);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionUnequipItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    bool instant = getIntAsBoolOrElse(args, 1, false);

    auto action = ctx.game.actionFactory().newUnequipItem(std::move(item), instant);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionAttack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto attackee = getObject(args, 0, ctx);
    bool passive = getIntAsBoolOrElse(args, 1, false);

    auto caller = getCallerAsCreature(ctx);
    auto action = ctx.game.actionFactory().newAttack(attackee, caller->getAttackRange(), false, passive);
    caller->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionPlayAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto animation = getIntAsEnum<AnimationType>(args, 0);
    float speed = getFloatOrElse(args, 1, 1.0f);
    float durationSeconds = getFloatOrElse(args, 2, 0.0f);

    auto action = ctx.game.actionFactory().newPlayAnimation(animation, speed, durationSeconds);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionOpenDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObjectAsDoor(args, 0, ctx);

    auto action = ctx.game.actionFactory().newOpenDoor(std::move(door));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionCloseDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto door = getObjectAsDoor(args, 0, ctx);

    auto action = ctx.game.actionFactory().newCloseDoor(std::move(door));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionCastSpellAtObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto target = getObject(args, 1, ctx);
    int metaMagic = getIntOrElse(args, 2, 0);
    bool cheat = getIntAsBoolOrElse(args, 3, false);
    int domainLevel = getIntOrElse(args, 4, 0);
    auto projectilePathType = getIntAsEnumOrElse(args, 5, ProjectilePathType::Default);
    bool instantSpell = getIntAsBoolOrElse(args, 6, false);

    auto action = ctx.game.actionFactory().newCastSpellAtObject(spell, std::move(target), metaMagic, cheat, domainLevel, projectilePathType, instantSpell);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionGiveItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto giveTo = getObject(args, 1, ctx);

    auto action = ctx.game.actionFactory().newGiveItem(std::move(item), std::move(giveTo));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionTakeItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto item = getObjectAsItem(args, 0, ctx);
    auto takeFrom = getObject(args, 1, ctx);

    auto action = ctx.game.actionFactory().newTakeItem(std::move(item), std::move(takeFrom));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionJumpToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto jumpTo = getObject(args, 0, ctx);
    bool walkStraightLine = getIntAsBoolOrElse(args, 1, true);

    auto action = ctx.game.actionFactory().newJumpToObject(std::move(jumpTo), walkStraightLine);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionWait(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float seconds = getFloat(args, 0);

    auto action = ctx.game.actionFactory().newWait(seconds);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionStartConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto objectToConverse = getObject(args, 0, ctx);
    std::string dialogResRef = getStringOrElse(args, 1, "");
    bool privateConversation = getIntAsBoolOrElse(args, 2, false);
    auto conversationType = getIntAsEnumOrElse(args, 3, ConversationType::Cinematic);
    bool ignoreStartRange = getIntAsBoolOrElse(args, 4, false);
    std::string nameObjectToIgnore1 = getStringOrElse(args, 5, "");
    std::string nameObjectToIgnore2 = getStringOrElse(args, 6, "");
    std::string nameObjectToIgnore3 = getStringOrElse(args, 7, "");
    std::string nameObjectToIgnore4 = getStringOrElse(args, 8, "");
    std::string nameObjectToIgnore5 = getStringOrElse(args, 9, "");
    std::string nameObjectToIgnore6 = getStringOrElse(args, 10, "");
    bool useLeader = getIntAsBoolOrElse(args, 11, false);
    int barkX = getIntOrElse(args, 12, -1);
    int barkY = getIntOrElse(args, 13, -1);
    bool dontClearAllActions = getIntAsBoolOrElse(args, 14, false);

    auto caller = getCaller(ctx);
    if (dialogResRef.empty()) {
        dialogResRef = caller->conversation();
    }
    auto action = ctx.game.actionFactory().newStartConversation(
        std::move(objectToConverse),
        std::move(dialogResRef),
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
    caller->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionPauseConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newPauseConversation();
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionResumeConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newResumeConversation();
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionJumpToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto location = getLocationArgument(args, 0);

    auto action = ctx.game.actionFactory().newJumpToLocation(std::move(location));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionUseSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto skill = getIntAsEnum<SkillType>(args, 0);
    auto target = getObject(args, 1, ctx);
    int subSkill = getIntOrElse(args, 2, 0);
    auto itemUsed = getObjectAsItemOrNull(args, 3, ctx);

    auto action = ctx.game.actionFactory().newUseSkill(std::move(target), skill, subSkill, std::move(itemUsed));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionDoCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto action = getAction(args, 0);

    auto commandAction = ctx.game.actionFactory().newDoCommand(std::move(action));
    getCaller(ctx)->addAction(std::move(commandAction));

    return Variable::ofNull();
}

Variable actionUseTalentOnObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto chosenTalent = getTalent(args, 0);
    auto target = getObject(args, 1, ctx);

    auto action = ctx.game.actionFactory().newUseTalentOnObject(std::move(chosenTalent), std::move(target));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionInteractObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto placeable = getObjectAsPlaceable(args, 0, ctx);

    auto action = ctx.game.actionFactory().newInteractObject(std::move(placeable));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionSurrenderToEnemies(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newSurrenderToEnemies();
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto destination = getLocationArgument(args, 0);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float timeout = getFloatOrElse(args, 2, 30.0f);

    auto action = ctx.game.actionFactory().newMoveToLocation(std::move(destination), run, timeout, true);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionForceMoveToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto moveTo = getObject(args, 0, ctx);
    bool run = getIntAsBoolOrElse(args, 1, false);
    float range = getFloatOrElse(args, 2, 1.0f);
    float timeout = getFloatOrElse(args, 3, 30.0f);

    auto action = ctx.game.actionFactory().newMoveToObject(std::move(moveTo), run, range, timeout, true);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingMelee(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto versus = getObjectOrNull(args, 0, ctx);
    bool offHand = getIntAsBoolOrElse(args, 1, false);

    auto action = ctx.game.actionFactory().newEquipMostDamagingMelee(std::move(versus), offHand);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionEquipMostDamagingRanged(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto versus = getObjectOrNull(args, 0, ctx);

    auto action = ctx.game.actionFactory().newEquipMostDamagingRanged(std::move(versus));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionUnlockObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);

    auto action = ctx.game.actionFactory().newUnlockObject(std::move(target));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionLockObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto target = getObject(args, 0, ctx);

    auto action = ctx.game.actionFactory().newLockObject(std::move(target));
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto target = getObject(args, 1, ctx);
    auto projectilePathType = getIntAsEnum<ProjectilePathType>(args, 2);

    auto action = ctx.game.actionFactory().newCastFakeSpellAtObject(spell, std::move(target), projectilePathType);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionCastFakeSpellAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto spell = getIntAsEnum<SpellType>(args, 0);
    auto location = getLocationArgument(args, 1);
    auto projectilePathType = getIntAsEnum<ProjectilePathType>(args, 2);

    auto action = ctx.game.actionFactory().newCastFakeSpellAtLocation(spell, std::move(location), projectilePathType);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionBarkString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    int strRef = getInt(args, 0);

    auto action = ctx.game.actionFactory().newBarkString(strRef);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionFollowLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newFollowLeader();
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

// TSL

Variable actionFollowOwner(const std::vector<Variable> &args, const RoutineContext &ctx) {
    float range = getFloatOrElse(args, 0, 2.5f);

    auto action = ctx.game.actionFactory().newFollowOwner(range);
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

Variable actionSwitchWeapons(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto action = ctx.game.actionFactory().newSwitchWeapons();
    getCaller(ctx)->addAction(std::move(action));

    return Variable::ofNull();
}

// END TSL

} // namespace routine

} // namespace game

} // namespace reone
