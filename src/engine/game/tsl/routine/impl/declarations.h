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

#pragma once

#include "../../../../script/variable.h"

namespace reone {

namespace game {

struct RoutineContext;

namespace routine {

#define R_ROUTINE(x) script::Variable(x)(const std::vector<script::Variable> &args, const RoutineContext &ctx);

R_ROUTINE(getScriptParameter)
R_ROUTINE(setFadeUntilScript)
R_ROUTINE(effectForceBody)
R_ROUTINE(getItemComponent)
R_ROUTINE(getItemComponentPieceValue)
R_ROUTINE(showChemicalUpgradeScreen)
R_ROUTINE(getChemicals)
R_ROUTINE(getChemicalPieceValue)
R_ROUTINE(getSpellForcePointCost)
R_ROUTINE(effectFury)
R_ROUTINE(effectBlind)
R_ROUTINE(effectFPRegenModifier)
R_ROUTINE(effectVPRegenModifier)
R_ROUTINE(effectCrush)
R_ROUTINE(getFeatAcquired)
R_ROUTINE(getSpellAcquired)
R_ROUTINE(showSwoopUpgradeScreen)
R_ROUTINE(grantFeat)
R_ROUTINE(grantSpell)
R_ROUTINE(spawnMine)
R_ROUTINE(setFakeCombatState)
R_ROUTINE(getOwnerDemolitionsSkill)
R_ROUTINE(setOrientOnClick)
R_ROUTINE(getInfluence)
R_ROUTINE(setInfluence)
R_ROUTINE(modifyInfluence)
R_ROUTINE(getRacialSubType)
R_ROUTINE(incrementGlobalNumber)
R_ROUTINE(decrementGlobalNumber)
R_ROUTINE(setBonusForcePoints)
R_ROUTINE(addBonusForcePoints)
R_ROUTINE(getBonusForcePoints)
R_ROUTINE(effectDroidConfused)
R_ROUTINE(isStealthed)
R_ROUTINE(isMeditating)
R_ROUTINE(isInTotalDefense)
R_ROUTINE(setHealTarget)
R_ROUTINE(getHealTarget)
R_ROUTINE(getRandomDestination)
R_ROUTINE(isFormActive)
R_ROUTINE(getSpellFormMask)
R_ROUTINE(getSpellBaseForcePointCost)
R_ROUTINE(setKeepStealthInDialog)
R_ROUTINE(hasLineOfSight)
R_ROUTINE(showDemoScreen)
R_ROUTINE(forceHeartbeat)
R_ROUTINE(effectForceSight)
R_ROUTINE(isRunning)
R_ROUTINE(setForfeitConditions)
R_ROUTINE(getLastForfeitViolation)
R_ROUTINE(modifyReflexSavingThrowBase)
R_ROUTINE(modifyFortitudeSavingThrowBase)
R_ROUTINE(modifyWillSavingThrowBase)
R_ROUTINE(getScriptStringParameter)
R_ROUTINE(getObjectPersonalSpace)
R_ROUTINE(adjustCreatureAttributes)
R_ROUTINE(setCreatureAILevel)
R_ROUTINE(resetCreatureAILevel)
R_ROUTINE(addAvailablePUPByTemplate)
R_ROUTINE(addAvailablePUPByObject)
R_ROUTINE(assignPUP)
R_ROUTINE(spawnAvailablePUP)
R_ROUTINE(addPartyPuppet)
R_ROUTINE(getPUPOwner)
R_ROUTINE(getIsPuppet)
R_ROUTINE(actionFollowOwner)
R_ROUTINE(getIsPartyLeader)
R_ROUTINE(getPartyLeader)
R_ROUTINE(removeNPCFromPartyToBase)
R_ROUTINE(creatureFlourishWeapon)
R_ROUTINE(effectMindTrick)
R_ROUTINE(effectFactionModifier)
R_ROUTINE(changeObjectAppearance)
R_ROUTINE(getIsXBox)
R_ROUTINE(effectDroidScramble)
R_ROUTINE(actionSwitchWeapons)
R_ROUTINE(playOverlayAnimation)
R_ROUTINE(unlockAllSongs)
R_ROUTINE(disableMap)
R_ROUTINE(detonateMine)
R_ROUTINE(disableHealthRegen)
R_ROUTINE(setCurrentForm)
R_ROUTINE(setDisableTransit)
R_ROUTINE(setInputClass)
R_ROUTINE(setForceAlwaysUpdate)
R_ROUTINE(enableRain)
R_ROUTINE(displayMessageBox)
R_ROUTINE(displayDatapad)
R_ROUTINE(removeHeartbeat)
R_ROUTINE(removeEffectByID)
R_ROUTINE(removeEffectByExactMatch)
R_ROUTINE(adjustCreatureSkills)
R_ROUTINE(getSkillRankBase)
R_ROUTINE(enableRendering)
R_ROUTINE(getCombatActionsPending)
R_ROUTINE(saveNPCByObject)
R_ROUTINE(savePUPByObject)
R_ROUTINE(getIsPlayerMadeCharacter)
R_ROUTINE(rebuildPartyTable)

namespace minigame {

R_ROUTINE(getSwoopUpgrade)
R_ROUTINE(getTrackPosition)
R_ROUTINE(setFollowerPosition)
R_ROUTINE(destroyMiniGameObject)
R_ROUTINE(setJumpSpeed)
R_ROUTINE(playerApplyForce)

} // namespace minigame

} // namespace routine

} // namespace game

} // namespace reone
