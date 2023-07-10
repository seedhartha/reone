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

#include "reone/game/game.h"
#include "reone/game/script/routine/argutil.h"
#include "reone/game/script/routine/context.h"
#include "reone/game/script/routines.h"
#include "reone/script/routine/exception/notimplemented.h"
#include "reone/script/variable.h"

#define R_VOID script::VariableType::Void
#define R_INT script::VariableType::Int
#define R_FLOAT script::VariableType::Float
#define R_OBJECT script::VariableType::Object
#define R_STRING script::VariableType::String
#define R_EFFECT script::VariableType::Effect
#define R_EVENT script::VariableType::Event
#define R_LOCATION script::VariableType::Location
#define R_TALENT script::VariableType::Talent
#define R_VECTOR script::VariableType::Vector
#define R_ACTION script::VariableType::Action

using namespace reone::script;

namespace reone {

namespace game {

static Variable ActionRandomWalk(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto action = ctx.game.actionFactory().newRandomWalk();
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionMoveToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lDestination = getLocationArgument(args, 0);
    auto bRun = getIntOrElse(args, 1, 0);

    // Transform
    auto run = static_cast<bool>(bRun);

    // Execute
    auto action = ctx.game.actionFactory().newMoveToLocation(std::move(lDestination), run);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionMoveToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMoveTo = getObject(args, 0, ctx);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fRange = getFloatOrElse(args, 2, 1.0f);

    // Transform
    auto run = static_cast<bool>(bRun);

    // Execute
    auto action = ctx.game.actionFactory().newMoveToObject(std::move(oMoveTo), run, fRange);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionMoveAwayFromObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFleeFrom = getObject(args, 0, ctx);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fMoveAwayRange = getFloatOrElse(args, 2, 40.0f);

    // Transform
    auto run = static_cast<bool>(bRun);

    // Execute
    auto action = ctx.game.actionFactory().newMoveAwayFromObject(std::move(oFleeFrom), run, fMoveAwayRange);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionEquipItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto nInventorySlot = getInt(args, 1);
    auto bInstant = getIntOrElse(args, 2, 0);

    // Transform
    auto item = checkItem(oItem);
    auto instant = static_cast<bool>(bInstant);

    // Execute
    auto action = ctx.game.actionFactory().newEquipItem(std::move(item), nInventorySlot, instant);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionUnequipItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto bInstant = getIntOrElse(args, 1, 0);

    // Transform
    auto item = checkItem(oItem);
    auto instant = static_cast<bool>(bInstant);

    // Execute
    auto action = ctx.game.actionFactory().newUnequipItem(std::move(item), instant);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionPickUpItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    auto action = ctx.game.actionFactory().newPickUpItem(std::move(item));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionPutDownItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    auto action = ctx.game.actionFactory().newPutDownItem(std::move(item));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionAttack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttackee = getObject(args, 0, ctx);
    auto bPassive = getIntOrElse(args, 1, 0);

    // Transform
    auto passive = static_cast<bool>(bPassive);

    // Execute
    auto caller = checkCreature(getCaller(ctx));
    auto action = ctx.game.actionFactory().newAttackObject(std::move(oAttackee), passive);
    caller->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionSpeakString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sStringToSpeak = getString(args, 0);
    auto nTalkVolume = getIntOrElse(args, 1, 0);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newSpeakString(sStringToSpeak, nTalkVolume);
    auto caller = getCaller(ctx);
    caller->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionPlayAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAnimation = getInt(args, 0);
    auto fSpeed = getFloatOrElse(args, 1, 1.0f);
    auto fDurationSeconds = getFloatOrElse(args, 2, 0.0f);

    // Transform
    auto animation = static_cast<AnimationType>(nAnimation);

    // Execute
    auto action = ctx.game.actionFactory().newPlayAnimation(animation, fSpeed, fDurationSeconds);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionOpenDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oDoor = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newOpenDoor(std::move(oDoor));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionCloseDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oDoor = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newCloseDoor(std::move(oDoor));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionCastSpellAtObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nMetaMagic = getIntOrElse(args, 2, 0);
    auto bCheat = getIntOrElse(args, 3, 0);
    auto nDomainLevel = getIntOrElse(args, 4, 0);
    auto nProjectilePathType = getIntOrElse(args, 5, 0);
    auto bInstantSpell = getIntOrElse(args, 6, 0);

    // Transform
    auto spell = static_cast<SpellType>(nSpell);
    auto cheat = static_cast<bool>(bCheat);
    auto projectilePathType = static_cast<ProjectilePathType>(nProjectilePathType);
    auto instantSpell = static_cast<bool>(bInstantSpell);

    // Execute
    auto action = ctx.game.actionFactory().newCastSpellAtObject(spell, std::move(oTarget), nMetaMagic, cheat, nDomainLevel, projectilePathType, instantSpell);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionGiveItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto oGiveTo = getObject(args, 1, ctx);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    auto action = ctx.game.actionFactory().newGiveItem(std::move(item), std::move(oGiveTo));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionTakeItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto oTakeFrom = getObject(args, 1, ctx);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    auto action = ctx.game.actionFactory().newTakeItem(std::move(item), std::move(oTakeFrom));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionForceFollowObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFollow = getObject(args, 0, ctx);
    auto fFollowDistance = getFloatOrElse(args, 1, 0.0f);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newFollow(oFollow, fFollowDistance);
    auto caller = getCaller(ctx);
    caller->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionJumpToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oToJumpTo = getObject(args, 0, ctx);
    auto bWalkStraightLineToPoint = getIntOrElse(args, 1, 1);

    // Transform
    auto walkStraightLine = static_cast<bool>(bWalkStraightLineToPoint);

    // Execute
    auto action = ctx.game.actionFactory().newJumpToObject(std::move(oToJumpTo), walkStraightLine);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionWait(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fSeconds = getFloat(args, 0);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newWait(fSeconds);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionStartConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObjectToConverse = getObject(args, 0, ctx);
    auto sDialogResRef = getStringOrElse(args, 1, "");
    auto bPrivateConversation = getIntOrElse(args, 2, 0);
    auto nConversationType = getIntOrElse(args, 3, 0);
    auto bIgnoreStartRange = getIntOrElse(args, 4, 0);
    auto sNameObjectToIgnore1 = getStringOrElse(args, 5, "");
    auto sNameObjectToIgnore2 = getStringOrElse(args, 6, "");
    auto sNameObjectToIgnore3 = getStringOrElse(args, 7, "");
    auto sNameObjectToIgnore4 = getStringOrElse(args, 8, "");
    auto sNameObjectToIgnore5 = getStringOrElse(args, 9, "");
    auto sNameObjectToIgnore6 = getStringOrElse(args, 10, "");
    auto bUseLeader = getIntOrElse(args, 11, 0);
    auto nBarkX = getIntOrElse(args, 12, -1);
    auto nBarkY = getIntOrElse(args, 13, -1);
    auto bDontClearAllActions = getIntOrElse(args, 14, 0);

    // Transform
    std::string dialogResRef;
    auto caller = getCaller(ctx);
    if (!sDialogResRef.empty()) {
        dialogResRef = sDialogResRef;
    } else {
        dialogResRef = caller->conversation();
    }
    auto privateConversation = static_cast<bool>(bPrivateConversation);
    auto conversationType = static_cast<ConversationType>(nConversationType);
    auto ignoreStartRange = static_cast<bool>(bIgnoreStartRange);
    auto namesToIgnore = std::vector<std::string> {sNameObjectToIgnore1,
                                                   sNameObjectToIgnore2,
                                                   sNameObjectToIgnore3,
                                                   sNameObjectToIgnore4,
                                                   sNameObjectToIgnore5,
                                                   sNameObjectToIgnore6};
    auto useLeader = static_cast<bool>(bUseLeader);
    auto dontClearAllActions = static_cast<bool>(bDontClearAllActions);

    // Execute
    auto action = ctx.game.actionFactory().newStartConversation(
        std::move(oObjectToConverse),
        dialogResRef,
        privateConversation,
        conversationType,
        ignoreStartRange,
        namesToIgnore,
        useLeader,
        nBarkX,
        nBarkY,
        dontClearAllActions);
    caller->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionPauseConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto action = ctx.game.actionFactory().newPauseConversation();
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionResumeConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto action = ctx.game.actionFactory().newResumeConversation();
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionJumpToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lLocation = getLocationArgument(args, 0);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newJumpToLocation(std::move(lLocation));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionCastSpellAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto lTargetLocation = getLocationArgument(args, 1);
    auto nMetaMagic = getIntOrElse(args, 2, 0);
    auto bCheat = getIntOrElse(args, 3, 0);
    auto nProjectilePathType = getIntOrElse(args, 4, 0);
    auto bInstantSpell = getIntOrElse(args, 5, 0);

    // Transform
    auto spell = static_cast<SpellType>(nSpell);
    auto cheat = static_cast<bool>(bCheat);
    auto projectilePathType = static_cast<ProjectilePathType>(nProjectilePathType);
    auto instantSpell = static_cast<bool>(bInstantSpell);

    // Execute
    auto action = ctx.game.actionFactory().newCastSpellAtLocation(spell, lTargetLocation, nMetaMagic, cheat, projectilePathType, instantSpell);
    auto caller = getCaller(ctx);
    caller->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionSpeakStringByStrRef(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStrRef = getInt(args, 0);
    auto nTalkVolume = getIntOrElse(args, 1, 0);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newSpeakStringByStrRef(nStrRef, nTalkVolume);
    auto caller = getCaller(ctx);
    caller->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionUseFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);

    // Transform
    auto feat = static_cast<FeatType>(nFeat);

    // Execute
    auto action = ctx.game.actionFactory().newUseFeat(feat, oTarget);
    auto caller = getCaller(ctx);
    caller->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionUseSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nSubSkill = getIntOrElse(args, 2, 0);
    auto oItemUsed = getObjectOrNull(args, 3, ctx);

    // Transform
    auto skill = static_cast<SkillType>(nSkill);
    auto itemUsed = checkItem(oItemUsed);

    // Execute
    auto action = ctx.game.actionFactory().newUseSkill(skill, std::move(oTarget), nSubSkill, std::move(itemUsed));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionDoCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto aActionToDo = getAction(args, 0);

    // Transform

    // Execute
    auto commandAction = ctx.game.actionFactory().newDoCommand(std::move(aActionToDo));
    getCaller(ctx)->addAction(std::move(commandAction));
    return Variable::ofNull();
}

static Variable ActionUseTalentOnObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tChosenTalent = getTalent(args, 0);
    auto oTarget = getObject(args, 1, ctx);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newUseTalentOnObject(tChosenTalent, oTarget);
    auto caller = getCaller(ctx);
    caller->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionUseTalentAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tChosenTalent = getTalent(args, 0);
    auto lTargetLocation = getLocationArgument(args, 1);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newUseTalentAtLocation(tChosenTalent, lTargetLocation);
    auto caller = getCaller(ctx);
    caller->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionInteractObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObject(args, 0, ctx);

    // Transform
    auto placeable = checkPlaceable(oPlaceable);

    // Execute
    auto action = ctx.game.actionFactory().newInteractObject(std::move(placeable));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionMoveAwayFromLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lMoveAwayFrom = getLocationArgument(args, 0);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fMoveAwayRange = getFloatOrElse(args, 2, 40.0f);

    // Transform
    auto run = static_cast<bool>(bRun);

    // Execute
    auto action = ctx.game.actionFactory().newMoveAwayFromLocation(std::move(lMoveAwayFrom), run, fMoveAwayRange);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionSurrenderToEnemies(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto action = ctx.game.actionFactory().newSurrenderToEnemies();
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionForceMoveToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lDestination = getLocationArgument(args, 0);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fTimeout = getFloatOrElse(args, 2, 30.0f);

    // Transform
    auto run = static_cast<bool>(bRun);

    // Execute
    auto action = ctx.game.actionFactory().newMoveToLocation(std::move(lDestination), run, true, fTimeout);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionForceMoveToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMoveTo = getObject(args, 0, ctx);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fRange = getFloatOrElse(args, 2, 1.0f);
    auto fTimeout = getFloatOrElse(args, 3, 30.0f);

    // Transform
    auto run = static_cast<bool>(bRun);

    // Execute
    auto action = ctx.game.actionFactory().newMoveToObject(std::move(oMoveTo), run, fRange, true, fTimeout);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionEquipMostDamagingMelee(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oVersus = getObjectOrNull(args, 0, ctx);
    auto bOffHand = getIntOrElse(args, 1, 0);

    // Transform
    auto offHand = static_cast<bool>(bOffHand);

    // Execute
    auto action = ctx.game.actionFactory().newEquipMostDamagingMelee(std::move(oVersus), offHand);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionEquipMostDamagingRanged(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oVersus = getObjectOrNull(args, 0, ctx);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newEquipMostDamagingRanged(std::move(oVersus));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionEquipMostEffectiveArmor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto action = ctx.game.actionFactory().newEquipMostEffectiveArmor();
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionUnlockObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newUnlockObject(std::move(oTarget));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionLockObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newLockObject(std::move(oTarget));
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionCastFakeSpellAtObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nProjectilePathType = getIntOrElse(args, 2, 0);

    // Transform
    auto spell = static_cast<SpellType>(nSpell);
    auto projectilePathType = static_cast<ProjectilePathType>(nProjectilePathType);

    // Execute
    auto action = ctx.game.actionFactory().newCastFakeSpellAtObject(spell, std::move(oTarget), projectilePathType);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionCastFakeSpellAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto lTarget = getLocationArgument(args, 1);
    auto nProjectilePathType = getIntOrElse(args, 2, 0);

    // Transform
    auto spell = static_cast<SpellType>(nSpell);
    auto projectilePathType = static_cast<ProjectilePathType>(nProjectilePathType);

    // Execute
    auto action = ctx.game.actionFactory().newCastFakeSpellAtLocation(spell, std::move(lTarget), projectilePathType);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionBarkString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto strRef = getInt(args, 0);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newBarkString(strRef);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionFollowLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto action = ctx.game.actionFactory().newFollowLeader();
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionFollowOwner(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fRange = getFloatOrElse(args, 0, 2.5f);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newFollowOwner(fRange);
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

static Variable ActionSwitchWeapons(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto action = ctx.game.actionFactory().newSwitchWeapons();
    getCaller(ctx)->addAction(std::move(action));
    return Variable::ofNull();
}

void Routines::registerActionKotorRoutines() {
    insert(20, "ActionRandomWalk", R_VOID, {}, &ActionRandomWalk);
    insert(21, "ActionMoveToLocation", R_VOID, {R_LOCATION, R_INT}, &ActionMoveToLocation);
    insert(22, "ActionMoveToObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT}, &ActionMoveToObject);
    insert(23, "ActionMoveAwayFromObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT}, &ActionMoveAwayFromObject);
    insert(32, "ActionEquipItem", R_VOID, {R_OBJECT, R_INT, R_INT}, &ActionEquipItem);
    insert(33, "ActionUnequipItem", R_VOID, {R_OBJECT, R_INT}, &ActionUnequipItem);
    insert(34, "ActionPickUpItem", R_VOID, {R_OBJECT}, &ActionPickUpItem);
    insert(35, "ActionPutDownItem", R_VOID, {R_OBJECT}, &ActionPutDownItem);
    insert(37, "ActionAttack", R_VOID, {R_OBJECT, R_INT}, &ActionAttack);
    insert(39, "ActionSpeakString", R_VOID, {R_STRING, R_INT}, &ActionSpeakString);
    insert(40, "ActionPlayAnimation", R_VOID, {R_INT, R_FLOAT, R_FLOAT}, &ActionPlayAnimation);
    insert(43, "ActionOpenDoor", R_VOID, {R_OBJECT}, &ActionOpenDoor);
    insert(44, "ActionCloseDoor", R_VOID, {R_OBJECT}, &ActionCloseDoor);
    insert(48, "ActionCastSpellAtObject", R_VOID, {R_INT, R_OBJECT, R_INT, R_INT, R_INT, R_INT, R_INT}, &ActionCastSpellAtObject);
    insert(135, "ActionGiveItem", R_VOID, {R_OBJECT, R_OBJECT}, &ActionGiveItem);
    insert(136, "ActionTakeItem", R_VOID, {R_OBJECT, R_OBJECT}, &ActionTakeItem);
    insert(167, "ActionForceFollowObject", R_VOID, {R_OBJECT, R_FLOAT}, &ActionForceFollowObject);
    insert(196, "ActionJumpToObject", R_VOID, {R_OBJECT, R_INT}, &ActionJumpToObject);
    insert(202, "ActionWait", R_VOID, {R_FLOAT}, &ActionWait);
    insert(204, "ActionStartConversation", R_VOID, {R_OBJECT, R_STRING, R_INT, R_INT, R_INT, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_INT}, &ActionStartConversation);
    insert(205, "ActionPauseConversation", R_VOID, {}, &ActionPauseConversation);
    insert(206, "ActionResumeConversation", R_VOID, {}, &ActionResumeConversation);
    insert(214, "ActionJumpToLocation", R_VOID, {R_LOCATION}, &ActionJumpToLocation);
    insert(234, "ActionCastSpellAtLocation", R_VOID, {R_INT, R_LOCATION, R_INT, R_INT, R_INT, R_INT}, &ActionCastSpellAtLocation);
    insert(240, "ActionSpeakStringByStrRef", R_VOID, {R_INT, R_INT}, &ActionSpeakStringByStrRef);
    insert(287, "ActionUseFeat", R_VOID, {R_INT, R_OBJECT}, &ActionUseFeat);
    insert(288, "ActionUseSkill", R_VOID, {R_INT, R_OBJECT, R_INT, R_OBJECT}, &ActionUseSkill);
    insert(294, "ActionDoCommand", R_VOID, {R_ACTION}, &ActionDoCommand);
    insert(309, "ActionUseTalentOnObject", R_VOID, {R_TALENT, R_OBJECT}, &ActionUseTalentOnObject);
    insert(310, "ActionUseTalentAtLocation", R_VOID, {R_TALENT, R_LOCATION}, &ActionUseTalentAtLocation);
    insert(329, "ActionInteractObject", R_VOID, {R_OBJECT}, &ActionInteractObject);
    insert(360, "ActionMoveAwayFromLocation", R_VOID, {R_LOCATION, R_INT, R_FLOAT}, &ActionMoveAwayFromLocation);
    insert(379, "ActionSurrenderToEnemies", R_VOID, {}, &ActionSurrenderToEnemies);
    insert(382, "ActionForceMoveToLocation", R_VOID, {R_LOCATION, R_INT, R_FLOAT}, &ActionForceMoveToLocation);
    insert(383, "ActionForceMoveToObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT, R_FLOAT}, &ActionForceMoveToObject);
    insert(399, "ActionEquipMostDamagingMelee", R_VOID, {R_OBJECT, R_INT}, &ActionEquipMostDamagingMelee);
    insert(400, "ActionEquipMostDamagingRanged", R_VOID, {R_OBJECT}, &ActionEquipMostDamagingRanged);
    insert(404, "ActionEquipMostEffectiveArmor", R_VOID, {}, &ActionEquipMostEffectiveArmor);
    insert(483, "ActionUnlockObject", R_VOID, {R_OBJECT}, &ActionUnlockObject);
    insert(484, "ActionLockObject", R_VOID, {R_OBJECT}, &ActionLockObject);
    insert(501, "ActionCastFakeSpellAtObject", R_VOID, {R_INT, R_OBJECT, R_INT}, &ActionCastFakeSpellAtObject);
    insert(502, "ActionCastFakeSpellAtLocation", R_VOID, {R_INT, R_LOCATION, R_INT}, &ActionCastFakeSpellAtLocation);
    insert(700, "ActionBarkString", R_VOID, {R_INT}, &ActionBarkString);
    insert(730, "ActionFollowLeader", R_VOID, {}, &ActionFollowLeader);
}

void Routines::registerActionTslRoutines() {
    insert(20, "ActionRandomWalk", R_VOID, {}, &ActionRandomWalk);
    insert(21, "ActionMoveToLocation", R_VOID, {R_LOCATION, R_INT}, &ActionMoveToLocation);
    insert(22, "ActionMoveToObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT}, &ActionMoveToObject);
    insert(23, "ActionMoveAwayFromObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT}, &ActionMoveAwayFromObject);
    insert(32, "ActionEquipItem", R_VOID, {R_OBJECT, R_INT, R_INT}, &ActionEquipItem);
    insert(33, "ActionUnequipItem", R_VOID, {R_OBJECT, R_INT}, &ActionUnequipItem);
    insert(34, "ActionPickUpItem", R_VOID, {R_OBJECT}, &ActionPickUpItem);
    insert(35, "ActionPutDownItem", R_VOID, {R_OBJECT}, &ActionPutDownItem);
    insert(37, "ActionAttack", R_VOID, {R_OBJECT, R_INT}, &ActionAttack);
    insert(39, "ActionSpeakString", R_VOID, {R_STRING, R_INT}, &ActionSpeakString);
    insert(40, "ActionPlayAnimation", R_VOID, {R_INT, R_FLOAT, R_FLOAT}, &ActionPlayAnimation);
    insert(43, "ActionOpenDoor", R_VOID, {R_OBJECT}, &ActionOpenDoor);
    insert(44, "ActionCloseDoor", R_VOID, {R_OBJECT}, &ActionCloseDoor);
    insert(48, "ActionCastSpellAtObject", R_VOID, {R_INT, R_OBJECT, R_INT, R_INT, R_INT, R_INT, R_INT}, &ActionCastSpellAtObject);
    insert(135, "ActionGiveItem", R_VOID, {R_OBJECT, R_OBJECT}, &ActionGiveItem);
    insert(136, "ActionTakeItem", R_VOID, {R_OBJECT, R_OBJECT}, &ActionTakeItem);
    insert(167, "ActionForceFollowObject", R_VOID, {R_OBJECT, R_FLOAT}, &ActionForceFollowObject);
    insert(196, "ActionJumpToObject", R_VOID, {R_OBJECT, R_INT}, &ActionJumpToObject);
    insert(202, "ActionWait", R_VOID, {R_FLOAT}, &ActionWait);
    insert(204, "ActionStartConversation", R_VOID, {R_OBJECT, R_STRING, R_INT, R_INT, R_INT, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_INT, R_INT, R_INT, R_INT}, &ActionStartConversation);
    insert(205, "ActionPauseConversation", R_VOID, {}, &ActionPauseConversation);
    insert(206, "ActionResumeConversation", R_VOID, {}, &ActionResumeConversation);
    insert(214, "ActionJumpToLocation", R_VOID, {R_LOCATION}, &ActionJumpToLocation);
    insert(234, "ActionCastSpellAtLocation", R_VOID, {R_INT, R_LOCATION, R_INT, R_INT, R_INT, R_INT}, &ActionCastSpellAtLocation);
    insert(240, "ActionSpeakStringByStrRef", R_VOID, {R_INT, R_INT}, &ActionSpeakStringByStrRef);
    insert(287, "ActionUseFeat", R_VOID, {R_INT, R_OBJECT}, &ActionUseFeat);
    insert(288, "ActionUseSkill", R_VOID, {R_INT, R_OBJECT, R_INT, R_OBJECT}, &ActionUseSkill);
    insert(294, "ActionDoCommand", R_VOID, {R_ACTION}, &ActionDoCommand);
    insert(309, "ActionUseTalentOnObject", R_VOID, {R_TALENT, R_OBJECT}, &ActionUseTalentOnObject);
    insert(310, "ActionUseTalentAtLocation", R_VOID, {R_TALENT, R_LOCATION}, &ActionUseTalentAtLocation);
    insert(329, "ActionInteractObject", R_VOID, {R_OBJECT}, &ActionInteractObject);
    insert(360, "ActionMoveAwayFromLocation", R_VOID, {R_LOCATION, R_INT, R_FLOAT}, &ActionMoveAwayFromLocation);
    insert(379, "ActionSurrenderToEnemies", R_VOID, {}, &ActionSurrenderToEnemies);
    insert(382, "ActionForceMoveToLocation", R_VOID, {R_LOCATION, R_INT, R_FLOAT}, &ActionForceMoveToLocation);
    insert(383, "ActionForceMoveToObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT, R_FLOAT}, &ActionForceMoveToObject);
    insert(399, "ActionEquipMostDamagingMelee", R_VOID, {R_OBJECT, R_INT}, &ActionEquipMostDamagingMelee);
    insert(400, "ActionEquipMostDamagingRanged", R_VOID, {R_OBJECT}, &ActionEquipMostDamagingRanged);
    insert(404, "ActionEquipMostEffectiveArmor", R_VOID, {}, &ActionEquipMostEffectiveArmor);
    insert(483, "ActionUnlockObject", R_VOID, {R_OBJECT}, &ActionUnlockObject);
    insert(484, "ActionLockObject", R_VOID, {R_OBJECT}, &ActionLockObject);
    insert(501, "ActionCastFakeSpellAtObject", R_VOID, {R_INT, R_OBJECT, R_INT}, &ActionCastFakeSpellAtObject);
    insert(502, "ActionCastFakeSpellAtLocation", R_VOID, {R_INT, R_LOCATION, R_INT}, &ActionCastFakeSpellAtLocation);
    insert(700, "ActionBarkString", R_VOID, {R_INT}, &ActionBarkString);
    insert(730, "ActionFollowLeader", R_VOID, {}, &ActionFollowLeader);
    insert(843, "ActionFollowOwner", R_VOID, {R_FLOAT}, &ActionFollowOwner);
    insert(853, "ActionSwitchWeapons", R_VOID, {}, &ActionSwitchWeapons);
}

} // namespace game

} // namespace reone
