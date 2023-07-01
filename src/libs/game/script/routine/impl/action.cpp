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

namespace routine {

static Variable ActionRandomWalk(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("ActionRandomWalk");
}

static Variable ActionMoveToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto lDestination = getLocationArgument(args, 0);
    auto bRun = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("ActionMoveToLocation");
}

static Variable ActionMoveToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oMoveTo = getObject(args, 0, ctx);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fRange = getFloatOrElse(args, 2, 1.0f);

    throw RoutineNotImplementedException("ActionMoveToObject");
}

static Variable ActionMoveAwayFromObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oFleeFrom = getObject(args, 0, ctx);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fMoveAwayRange = getFloatOrElse(args, 2, 40.0f);

    throw RoutineNotImplementedException("ActionMoveAwayFromObject");
}

static Variable ActionEquipItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oItem = getObject(args, 0, ctx);
    auto nInventorySlot = getInt(args, 1);
    auto bInstant = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("ActionEquipItem");
}

static Variable ActionUnequipItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oItem = getObject(args, 0, ctx);
    auto bInstant = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("ActionUnequipItem");
}

static Variable ActionPickUpItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oItem = getObject(args, 0, ctx);

    throw RoutineNotImplementedException("ActionPickUpItem");
}

static Variable ActionPutDownItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oItem = getObject(args, 0, ctx);

    throw RoutineNotImplementedException("ActionPutDownItem");
}

static Variable ActionAttack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oAttackee = getObject(args, 0, ctx);
    auto bPassive = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("ActionAttack");
}

static Variable ActionSpeakString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto sStringToSpeak = getString(args, 0);
    auto nTalkVolume = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("ActionSpeakString");
}

static Variable ActionPlayAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nAnimation = getInt(args, 0);
    auto fSpeed = getFloatOrElse(args, 1, 1.0);
    auto fDurationSeconds = getFloatOrElse(args, 2, 0.0);

    throw RoutineNotImplementedException("ActionPlayAnimation");
}

static Variable ActionOpenDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oDoor = getObject(args, 0, ctx);

    throw RoutineNotImplementedException("ActionOpenDoor");
}

static Variable ActionCloseDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oDoor = getObject(args, 0, ctx);

    throw RoutineNotImplementedException("ActionCloseDoor");
}

static Variable ActionCastSpellAtObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSpell = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nMetaMagic = getIntOrElse(args, 2, 0);
    auto bCheat = getIntOrElse(args, 3, 0);
    auto nDomainLevel = getIntOrElse(args, 4, 0);
    auto nProjectilePathType = getIntOrElse(args, 5, 0);
    auto bInstantSpell = getIntOrElse(args, 6, 0);

    throw RoutineNotImplementedException("ActionCastSpellAtObject");
}

static Variable ActionGiveItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oItem = getObject(args, 0, ctx);
    auto oGiveTo = getObject(args, 1, ctx);

    throw RoutineNotImplementedException("ActionGiveItem");
}

static Variable ActionTakeItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oItem = getObject(args, 0, ctx);
    auto oTakeFrom = getObject(args, 1, ctx);

    throw RoutineNotImplementedException("ActionTakeItem");
}

static Variable ActionForceFollowObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oFollow = getObject(args, 0, ctx);
    auto fFollowDistance = getFloatOrElse(args, 1, 0.0f);

    throw RoutineNotImplementedException("ActionForceFollowObject");
}

static Variable ActionJumpToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oToJumpTo = getObject(args, 0, ctx);
    auto bWalkStraightLineToPoint = getIntOrElse(args, 1, 1);

    throw RoutineNotImplementedException("ActionJumpToObject");
}

static Variable ActionWait(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto fSeconds = getFloat(args, 0);

    throw RoutineNotImplementedException("ActionWait");
}

static Variable ActionStartConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
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

    throw RoutineNotImplementedException("ActionStartConversation");
}

static Variable ActionPauseConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("ActionPauseConversation");
}

static Variable ActionResumeConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("ActionResumeConversation");
}

static Variable ActionJumpToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto lLocation = getLocationArgument(args, 0);

    throw RoutineNotImplementedException("ActionJumpToLocation");
}

static Variable ActionCastSpellAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSpell = getInt(args, 0);
    auto lTargetLocation = getLocationArgument(args, 1);
    auto nMetaMagic = getIntOrElse(args, 2, 0);
    auto bCheat = getIntOrElse(args, 3, 0);
    auto nProjectilePathType = getIntOrElse(args, 4, 0);
    auto bInstantSpell = getIntOrElse(args, 5, 0);

    throw RoutineNotImplementedException("ActionCastSpellAtLocation");
}

static Variable ActionSpeakStringByStrRef(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nStrRef = getInt(args, 0);
    auto nTalkVolume = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("ActionSpeakStringByStrRef");
}

static Variable ActionUseFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nFeat = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);

    throw RoutineNotImplementedException("ActionUseFeat");
}

static Variable ActionUseSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSkill = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nSubSkill = getIntOrElse(args, 2, 0);
    auto oItemUsed = getObjectOrNull(args, 3, ctx);

    throw RoutineNotImplementedException("ActionUseSkill");
}

static Variable ActionDoCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto aActionToDo = getAction(args, 0);

    throw RoutineNotImplementedException("ActionDoCommand");
}

static Variable ActionUseTalentOnObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto tChosenTalent = getTalent(args, 0);
    auto oTarget = getObject(args, 1, ctx);

    throw RoutineNotImplementedException("ActionUseTalentOnObject");
}

static Variable ActionUseTalentAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto tChosenTalent = getTalent(args, 0);
    auto lTargetLocation = getLocationArgument(args, 1);

    throw RoutineNotImplementedException("ActionUseTalentAtLocation");
}

static Variable ActionInteractObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oPlaceable = getObject(args, 0, ctx);

    throw RoutineNotImplementedException("ActionInteractObject");
}

static Variable ActionMoveAwayFromLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto lMoveAwayFrom = getLocationArgument(args, 0);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fMoveAwayRange = getFloatOrElse(args, 2, 40.0f);

    throw RoutineNotImplementedException("ActionMoveAwayFromLocation");
}

static Variable ActionSurrenderToEnemies(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("ActionSurrenderToEnemies");
}

static Variable ActionForceMoveToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto lDestination = getLocationArgument(args, 0);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fTimeout = getFloatOrElse(args, 2, 30.0f);

    throw RoutineNotImplementedException("ActionForceMoveToLocation");
}

static Variable ActionForceMoveToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oMoveTo = getObject(args, 0, ctx);
    auto bRun = getIntOrElse(args, 1, 0);
    auto fRange = getFloatOrElse(args, 2, 1.0f);
    auto fTimeout = getFloatOrElse(args, 3, 30.0f);

    throw RoutineNotImplementedException("ActionForceMoveToObject");
}

static Variable ActionEquipMostDamagingMelee(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oVersus = getObjectOrNull(args, 0, ctx);
    auto bOffHand = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("ActionEquipMostDamagingMelee");
}

static Variable ActionEquipMostDamagingRanged(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oVersus = getObjectOrNull(args, 0, ctx);

    throw RoutineNotImplementedException("ActionEquipMostDamagingRanged");
}

static Variable ActionEquipMostEffectiveArmor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("ActionEquipMostEffectiveArmor");
}

static Variable ActionUnlockObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oTarget = getObject(args, 0, ctx);

    throw RoutineNotImplementedException("ActionUnlockObject");
}

static Variable ActionLockObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oTarget = getObject(args, 0, ctx);

    throw RoutineNotImplementedException("ActionLockObject");
}

static Variable ActionCastFakeSpellAtObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSpell = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nProjectilePathType = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("ActionCastFakeSpellAtObject");
}

static Variable ActionCastFakeSpellAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSpell = getInt(args, 0);
    auto lTarget = getLocationArgument(args, 1);
    auto nProjectilePathType = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("ActionCastFakeSpellAtLocation");
}

static Variable ActionBarkString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto strRef = getInt(args, 0);

    throw RoutineNotImplementedException("ActionBarkString");
}

static Variable ActionFollowLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("ActionFollowLeader");
}

static Variable ActionFollowOwner(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto fRange = getFloatOrElse(args, 0, 2.5);

    throw RoutineNotImplementedException("ActionFollowOwner");
}

static Variable ActionSwitchWeapons(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("ActionSwitchWeapons");
}

} // namespace routine

void registerActionKotorRoutines(Routines &routines) {
    routines.insert(20, "ActionRandomWalk", R_VOID, {}, &routine::ActionRandomWalk);
    routines.insert(21, "ActionMoveToLocation", R_VOID, {R_LOCATION, R_INT}, &routine::ActionMoveToLocation);
    routines.insert(22, "ActionMoveToObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT}, &routine::ActionMoveToObject);
    routines.insert(23, "ActionMoveAwayFromObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT}, &routine::ActionMoveAwayFromObject);
    routines.insert(32, "ActionEquipItem", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::ActionEquipItem);
    routines.insert(33, "ActionUnequipItem", R_VOID, {R_OBJECT, R_INT}, &routine::ActionUnequipItem);
    routines.insert(34, "ActionPickUpItem", R_VOID, {R_OBJECT}, &routine::ActionPickUpItem);
    routines.insert(35, "ActionPutDownItem", R_VOID, {R_OBJECT}, &routine::ActionPutDownItem);
    routines.insert(37, "ActionAttack", R_VOID, {R_OBJECT, R_INT}, &routine::ActionAttack);
    routines.insert(39, "ActionSpeakString", R_VOID, {R_STRING, R_INT}, &routine::ActionSpeakString);
    routines.insert(40, "ActionPlayAnimation", R_VOID, {R_INT, R_FLOAT, R_FLOAT}, &routine::ActionPlayAnimation);
    routines.insert(43, "ActionOpenDoor", R_VOID, {R_OBJECT}, &routine::ActionOpenDoor);
    routines.insert(44, "ActionCloseDoor", R_VOID, {R_OBJECT}, &routine::ActionCloseDoor);
    routines.insert(48, "ActionCastSpellAtObject", R_VOID, {R_INT, R_OBJECT, R_INT, R_INT, R_INT, R_INT, R_INT}, &routine::ActionCastSpellAtObject);
    routines.insert(135, "ActionGiveItem", R_VOID, {R_OBJECT, R_OBJECT}, &routine::ActionGiveItem);
    routines.insert(136, "ActionTakeItem", R_VOID, {R_OBJECT, R_OBJECT}, &routine::ActionTakeItem);
    routines.insert(167, "ActionForceFollowObject", R_VOID, {R_OBJECT, R_FLOAT}, &routine::ActionForceFollowObject);
    routines.insert(196, "ActionJumpToObject", R_VOID, {R_OBJECT, R_INT}, &routine::ActionJumpToObject);
    routines.insert(202, "ActionWait", R_VOID, {R_FLOAT}, &routine::ActionWait);
    routines.insert(204, "ActionStartConversation", R_VOID, {R_OBJECT, R_STRING, R_INT, R_INT, R_INT, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_INT}, &routine::ActionStartConversation);
    routines.insert(205, "ActionPauseConversation", R_VOID, {}, &routine::ActionPauseConversation);
    routines.insert(206, "ActionResumeConversation", R_VOID, {}, &routine::ActionResumeConversation);
    routines.insert(214, "ActionJumpToLocation", R_VOID, {R_LOCATION}, &routine::ActionJumpToLocation);
    routines.insert(234, "ActionCastSpellAtLocation", R_VOID, {R_INT, R_LOCATION, R_INT, R_INT, R_INT, R_INT}, &routine::ActionCastSpellAtLocation);
    routines.insert(240, "ActionSpeakStringByStrRef", R_VOID, {R_INT, R_INT}, &routine::ActionSpeakStringByStrRef);
    routines.insert(287, "ActionUseFeat", R_VOID, {R_INT, R_OBJECT}, &routine::ActionUseFeat);
    routines.insert(288, "ActionUseSkill", R_VOID, {R_INT, R_OBJECT, R_INT, R_OBJECT}, &routine::ActionUseSkill);
    routines.insert(294, "ActionDoCommand", R_VOID, {R_ACTION}, &routine::ActionDoCommand);
    routines.insert(309, "ActionUseTalentOnObject", R_VOID, {R_TALENT, R_OBJECT}, &routine::ActionUseTalentOnObject);
    routines.insert(310, "ActionUseTalentAtLocation", R_VOID, {R_TALENT, R_LOCATION}, &routine::ActionUseTalentAtLocation);
    routines.insert(329, "ActionInteractObject", R_VOID, {R_OBJECT}, &routine::ActionInteractObject);
    routines.insert(360, "ActionMoveAwayFromLocation", R_VOID, {R_LOCATION, R_INT, R_FLOAT}, &routine::ActionMoveAwayFromLocation);
    routines.insert(379, "ActionSurrenderToEnemies", R_VOID, {}, &routine::ActionSurrenderToEnemies);
    routines.insert(382, "ActionForceMoveToLocation", R_VOID, {R_LOCATION, R_INT, R_FLOAT}, &routine::ActionForceMoveToLocation);
    routines.insert(383, "ActionForceMoveToObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT, R_FLOAT}, &routine::ActionForceMoveToObject);
    routines.insert(399, "ActionEquipMostDamagingMelee", R_VOID, {R_OBJECT, R_INT}, &routine::ActionEquipMostDamagingMelee);
    routines.insert(400, "ActionEquipMostDamagingRanged", R_VOID, {R_OBJECT}, &routine::ActionEquipMostDamagingRanged);
    routines.insert(404, "ActionEquipMostEffectiveArmor", R_VOID, {}, &routine::ActionEquipMostEffectiveArmor);
    routines.insert(483, "ActionUnlockObject", R_VOID, {R_OBJECT}, &routine::ActionUnlockObject);
    routines.insert(484, "ActionLockObject", R_VOID, {R_OBJECT}, &routine::ActionLockObject);
    routines.insert(501, "ActionCastFakeSpellAtObject", R_VOID, {R_INT, R_OBJECT, R_INT}, &routine::ActionCastFakeSpellAtObject);
    routines.insert(502, "ActionCastFakeSpellAtLocation", R_VOID, {R_INT, R_LOCATION, R_INT}, &routine::ActionCastFakeSpellAtLocation);
    routines.insert(700, "ActionBarkString", R_VOID, {R_INT}, &routine::ActionBarkString);
    routines.insert(730, "ActionFollowLeader", R_VOID, {}, &routine::ActionFollowLeader);
}

void registerActionTslRoutines(Routines &routines) {
    routines.insert(20, "ActionRandomWalk", R_VOID, {}, &routine::ActionRandomWalk);
    routines.insert(21, "ActionMoveToLocation", R_VOID, {R_LOCATION, R_INT}, &routine::ActionMoveToLocation);
    routines.insert(22, "ActionMoveToObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT}, &routine::ActionMoveToObject);
    routines.insert(23, "ActionMoveAwayFromObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT}, &routine::ActionMoveAwayFromObject);
    routines.insert(32, "ActionEquipItem", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::ActionEquipItem);
    routines.insert(33, "ActionUnequipItem", R_VOID, {R_OBJECT, R_INT}, &routine::ActionUnequipItem);
    routines.insert(34, "ActionPickUpItem", R_VOID, {R_OBJECT}, &routine::ActionPickUpItem);
    routines.insert(35, "ActionPutDownItem", R_VOID, {R_OBJECT}, &routine::ActionPutDownItem);
    routines.insert(37, "ActionAttack", R_VOID, {R_OBJECT, R_INT}, &routine::ActionAttack);
    routines.insert(39, "ActionSpeakString", R_VOID, {R_STRING, R_INT}, &routine::ActionSpeakString);
    routines.insert(40, "ActionPlayAnimation", R_VOID, {R_INT, R_FLOAT, R_FLOAT}, &routine::ActionPlayAnimation);
    routines.insert(43, "ActionOpenDoor", R_VOID, {R_OBJECT}, &routine::ActionOpenDoor);
    routines.insert(44, "ActionCloseDoor", R_VOID, {R_OBJECT}, &routine::ActionCloseDoor);
    routines.insert(48, "ActionCastSpellAtObject", R_VOID, {R_INT, R_OBJECT, R_INT, R_INT, R_INT, R_INT, R_INT}, &routine::ActionCastSpellAtObject);
    routines.insert(135, "ActionGiveItem", R_VOID, {R_OBJECT, R_OBJECT}, &routine::ActionGiveItem);
    routines.insert(136, "ActionTakeItem", R_VOID, {R_OBJECT, R_OBJECT}, &routine::ActionTakeItem);
    routines.insert(167, "ActionForceFollowObject", R_VOID, {R_OBJECT, R_FLOAT}, &routine::ActionForceFollowObject);
    routines.insert(196, "ActionJumpToObject", R_VOID, {R_OBJECT, R_INT}, &routine::ActionJumpToObject);
    routines.insert(202, "ActionWait", R_VOID, {R_FLOAT}, &routine::ActionWait);
    routines.insert(204, "ActionStartConversation", R_VOID, {R_OBJECT, R_STRING, R_INT, R_INT, R_INT, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_INT, R_INT, R_INT, R_INT}, &routine::ActionStartConversation);
    routines.insert(205, "ActionPauseConversation", R_VOID, {}, &routine::ActionPauseConversation);
    routines.insert(206, "ActionResumeConversation", R_VOID, {}, &routine::ActionResumeConversation);
    routines.insert(214, "ActionJumpToLocation", R_VOID, {R_LOCATION}, &routine::ActionJumpToLocation);
    routines.insert(234, "ActionCastSpellAtLocation", R_VOID, {R_INT, R_LOCATION, R_INT, R_INT, R_INT, R_INT}, &routine::ActionCastSpellAtLocation);
    routines.insert(240, "ActionSpeakStringByStrRef", R_VOID, {R_INT, R_INT}, &routine::ActionSpeakStringByStrRef);
    routines.insert(287, "ActionUseFeat", R_VOID, {R_INT, R_OBJECT}, &routine::ActionUseFeat);
    routines.insert(288, "ActionUseSkill", R_VOID, {R_INT, R_OBJECT, R_INT, R_OBJECT}, &routine::ActionUseSkill);
    routines.insert(294, "ActionDoCommand", R_VOID, {R_ACTION}, &routine::ActionDoCommand);
    routines.insert(309, "ActionUseTalentOnObject", R_VOID, {R_TALENT, R_OBJECT}, &routine::ActionUseTalentOnObject);
    routines.insert(310, "ActionUseTalentAtLocation", R_VOID, {R_TALENT, R_LOCATION}, &routine::ActionUseTalentAtLocation);
    routines.insert(329, "ActionInteractObject", R_VOID, {R_OBJECT}, &routine::ActionInteractObject);
    routines.insert(360, "ActionMoveAwayFromLocation", R_VOID, {R_LOCATION, R_INT, R_FLOAT}, &routine::ActionMoveAwayFromLocation);
    routines.insert(379, "ActionSurrenderToEnemies", R_VOID, {}, &routine::ActionSurrenderToEnemies);
    routines.insert(382, "ActionForceMoveToLocation", R_VOID, {R_LOCATION, R_INT, R_FLOAT}, &routine::ActionForceMoveToLocation);
    routines.insert(383, "ActionForceMoveToObject", R_VOID, {R_OBJECT, R_INT, R_FLOAT, R_FLOAT}, &routine::ActionForceMoveToObject);
    routines.insert(399, "ActionEquipMostDamagingMelee", R_VOID, {R_OBJECT, R_INT}, &routine::ActionEquipMostDamagingMelee);
    routines.insert(400, "ActionEquipMostDamagingRanged", R_VOID, {R_OBJECT}, &routine::ActionEquipMostDamagingRanged);
    routines.insert(404, "ActionEquipMostEffectiveArmor", R_VOID, {}, &routine::ActionEquipMostEffectiveArmor);
    routines.insert(483, "ActionUnlockObject", R_VOID, {R_OBJECT}, &routine::ActionUnlockObject);
    routines.insert(484, "ActionLockObject", R_VOID, {R_OBJECT}, &routine::ActionLockObject);
    routines.insert(501, "ActionCastFakeSpellAtObject", R_VOID, {R_INT, R_OBJECT, R_INT}, &routine::ActionCastFakeSpellAtObject);
    routines.insert(502, "ActionCastFakeSpellAtLocation", R_VOID, {R_INT, R_LOCATION, R_INT}, &routine::ActionCastFakeSpellAtLocation);
    routines.insert(700, "ActionBarkString", R_VOID, {R_INT}, &routine::ActionBarkString);
    routines.insert(730, "ActionFollowLeader", R_VOID, {}, &routine::ActionFollowLeader);
    routines.insert(843, "ActionFollowOwner", R_VOID, {R_FLOAT}, &routine::ActionFollowOwner);
    routines.insert(853, "ActionSwitchWeapons", R_VOID, {}, &routine::ActionSwitchWeapons);
}

} // namespace game

} // namespace reone

