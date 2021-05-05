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

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include "../../common/collectionutil.h"
#include "../../resource/types.h"
#include "../../script/routine.h"
#include "../../script/types.h"
#include "../../script/variable.h"

#define REO_DECL_ROUTINE(x) script::Variable (x)(const VariablesList &args, script::ExecutionContext &ctx);

namespace reone {

namespace game {

class Creature;
class Door;
class Effect;
class Event;
class Game;
class Item;
class Location;
class Object;
class Sound;
class SpatialObject;
class Talent;

class Routines : public script::IRoutineProvider, boost::noncopyable {
public:
    static Routines &instance();

    void init(Game *game);
    void deinit();

    const script::Routine &get(int index) override;

private:
    typedef std::vector<script::VariableType> VariableTypesList;
    typedef std::vector<script::Variable> VariablesList;

    Game *_game { nullptr };
    std::vector<script::Routine> _routines;

    ~Routines();

    void add(const std::string &name, script::VariableType retType, const VariableTypesList &argTypes);

    template <class T>
    void add(
        const std::string &name,
        script::VariableType retType,
        const VariableTypesList &argTypes,
        const T &fn) {

        _routines.emplace_back(name, retType, argTypes, std::bind(fn, this, std::placeholders::_1, std::placeholders::_2));
    }

    void addKotorRoutines();
    void addTslRoutines();

    // Helper functions

    bool getBool(const VariablesList &args, int index, bool defValue = false) const;
    int getInt(const VariablesList &args, int index, int defValue = 0) const;
    float getFloat(const VariablesList &args, int index, float defValue = 0.0f) const;
    std::string getString(const VariablesList &args, int index, std::string defValue = "") const;
    glm::vec3 getVector(const VariablesList &args, int index, glm::vec3 defValue = glm::vec3(0.0f)) const;
    std::shared_ptr<script::ExecutionContext> getAction(const VariablesList &args, int index) const;

    std::shared_ptr<Object> getCaller(script::ExecutionContext &ctx) const;
    std::shared_ptr<SpatialObject> getCallerAsSpatial(script::ExecutionContext &ctx) const;
    std::shared_ptr<Creature> getCallerAsCreature(script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getTriggerrer(script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getObject(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getObjectOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<SpatialObject> getSpatialObject(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<SpatialObject> getSpatialObjectOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Creature> getCreature(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Creature> getCreatureOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Door> getDoor(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Item> getItem(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Sound> getSound(const VariablesList &args, int index, script::ExecutionContext &ctx) const;

    std::shared_ptr<Effect> getEffect(const VariablesList &args, int index) const;
    std::shared_ptr<Event> getEvent(const VariablesList &args, int index) const;
    std::shared_ptr<Location> getLocationEngineType(const VariablesList &args, int index) const;
    std::shared_ptr<Talent> getTalent(const VariablesList &args, int index) const;

    template <class T>
    inline T getEnum(const VariablesList &args, int index) const {
        if (isOutOfRange(args, index)) {
            throw std::out_of_range("index is out of range");
        }
        return static_cast<T>(args[index].intValue);
    }

    template <class T>
    inline T getEnum(const VariablesList &args, int index, T defValue) const {
        return isOutOfRange(args, index) ?
            std::move(defValue) :
            static_cast<T>(args[index].intValue);
    }

    // END Helper functions

    // Routine implementations

    REO_DECL_ROUTINE(abs)
    REO_DECL_ROUTINE(acos)
    REO_DECL_ROUTINE(actionAttack)
    REO_DECL_ROUTINE(actionBarkString)
    REO_DECL_ROUTINE(actionCastFakeSpellAtLocation)
    REO_DECL_ROUTINE(actionCastFakeSpellAtObject)
    REO_DECL_ROUTINE(actionCastSpellAtLocation)
    REO_DECL_ROUTINE(actionCastSpellAtObject)
    REO_DECL_ROUTINE(actionCloseDoor)
    REO_DECL_ROUTINE(actionDoCommand)
    REO_DECL_ROUTINE(actionEquipItem)
    REO_DECL_ROUTINE(actionEquipMostDamagingMelee)
    REO_DECL_ROUTINE(actionEquipMostDamagingRanged)
    REO_DECL_ROUTINE(actionEquipMostEffectiveArmor)
    REO_DECL_ROUTINE(actionFollowLeader)
    REO_DECL_ROUTINE(actionFollowOwner)
    REO_DECL_ROUTINE(actionForceFollowObject)
    REO_DECL_ROUTINE(actionForceMoveToLocation)
    REO_DECL_ROUTINE(actionForceMoveToObject)
    REO_DECL_ROUTINE(actionGiveItem)
    REO_DECL_ROUTINE(actionInteractObject)
    REO_DECL_ROUTINE(actionJumpToLocation)
    REO_DECL_ROUTINE(actionJumpToObject)
    REO_DECL_ROUTINE(actionLockObject)
    REO_DECL_ROUTINE(actionMoveAwayFromLocation)
    REO_DECL_ROUTINE(actionMoveAwayFromObject)
    REO_DECL_ROUTINE(actionMoveToLocation)
    REO_DECL_ROUTINE(actionMoveToObject)
    REO_DECL_ROUTINE(actionOpenDoor)
    REO_DECL_ROUTINE(actionPauseConversation)
    REO_DECL_ROUTINE(actionPickUpItem)
    REO_DECL_ROUTINE(actionPlayAnimation)
    REO_DECL_ROUTINE(actionPutDownItem)
    REO_DECL_ROUTINE(actionRandomWalk)
    REO_DECL_ROUTINE(actionResumeConversation)
    REO_DECL_ROUTINE(actionSpeakString)
    REO_DECL_ROUTINE(actionSpeakStringByStrRef)
    REO_DECL_ROUTINE(actionStartConversation)
    REO_DECL_ROUTINE(actionSurrenderToEnemies)
    REO_DECL_ROUTINE(actionSwitchWeapons)
    REO_DECL_ROUTINE(actionTakeItem)
    REO_DECL_ROUTINE(actionUnequipItem)
    REO_DECL_ROUTINE(actionUnlockObject)
    REO_DECL_ROUTINE(actionUseFeat)
    REO_DECL_ROUTINE(actionUseSkill)
    REO_DECL_ROUTINE(actionUseTalentAtLocation)
    REO_DECL_ROUTINE(actionUseTalentOnObject)
    REO_DECL_ROUTINE(actionWait)
    REO_DECL_ROUTINE(addAvailableNPCByTemplate)
    REO_DECL_ROUTINE(addPartyMember)
    REO_DECL_ROUTINE(applyEffectToObject)
    REO_DECL_ROUTINE(asin)
    REO_DECL_ROUTINE(assignCommand)
    REO_DECL_ROUTINE(atan)
    REO_DECL_ROUTINE(changeFaction)
    REO_DECL_ROUTINE(changeToStandardFaction)
    REO_DECL_ROUTINE(clearAllActions)
    REO_DECL_ROUTINE(clearAllEffects)
    REO_DECL_ROUTINE(cos)
    REO_DECL_ROUTINE(createItemOnObject)
    REO_DECL_ROUTINE(createObject)
    REO_DECL_ROUTINE(cutsceneAttack)
    REO_DECL_ROUTINE(d10)
    REO_DECL_ROUTINE(d100)
    REO_DECL_ROUTINE(d12)
    REO_DECL_ROUTINE(d2)
    REO_DECL_ROUTINE(d20)
    REO_DECL_ROUTINE(d3)
    REO_DECL_ROUTINE(d4)
    REO_DECL_ROUTINE(d6)
    REO_DECL_ROUTINE(d8)
    REO_DECL_ROUTINE(delayCommand)
    REO_DECL_ROUTINE(destroyObject)
    REO_DECL_ROUTINE(effectAbilityDecrease)
    REO_DECL_ROUTINE(effectAbilityIncrease)
    REO_DECL_ROUTINE(effectACDecrease)
    REO_DECL_ROUTINE(effectACIncrease)
    REO_DECL_ROUTINE(effectAreaOfEffect)
    REO_DECL_ROUTINE(effectAssuredDeflection)
    REO_DECL_ROUTINE(effectAssuredHit)
    REO_DECL_ROUTINE(effectAttackDecrease)
    REO_DECL_ROUTINE(effectAttackIncrease)
    REO_DECL_ROUTINE(effectBeam)
    REO_DECL_ROUTINE(effectBlasterDeflectionDecrease)
    REO_DECL_ROUTINE(effectBlasterDeflectionIncrease)
    REO_DECL_ROUTINE(effectBlind)
    REO_DECL_ROUTINE(effectBodyFuel)
    REO_DECL_ROUTINE(effectChoke)
    REO_DECL_ROUTINE(effectConcealment)
    REO_DECL_ROUTINE(effectConfused)
    REO_DECL_ROUTINE(effectCrush)
    REO_DECL_ROUTINE(effectCutSceneHorrified)
    REO_DECL_ROUTINE(effectCutSceneParalyze)
    REO_DECL_ROUTINE(effectCutSceneStunned)
    REO_DECL_ROUTINE(effectDamage)
    REO_DECL_ROUTINE(effectDamageDecrease)
    REO_DECL_ROUTINE(effectDamageForcePoints)
    REO_DECL_ROUTINE(effectDamageImmunityDecrease)
    REO_DECL_ROUTINE(effectDamageImmunityIncrease)
    REO_DECL_ROUTINE(effectDamageIncrease)
    REO_DECL_ROUTINE(effectDamageReduction)
    REO_DECL_ROUTINE(effectDamageResistance)
    REO_DECL_ROUTINE(effectDamageShield)
    REO_DECL_ROUTINE(effectDeath)
    REO_DECL_ROUTINE(effectDisguise)
    REO_DECL_ROUTINE(effectDispelMagicAll)
    REO_DECL_ROUTINE(effectDispelMagicBest)
    REO_DECL_ROUTINE(effectDroidConfused)
    REO_DECL_ROUTINE(effectDroidScramble)
    REO_DECL_ROUTINE(effectDroidStun)
    REO_DECL_ROUTINE(effectEntangle)
    REO_DECL_ROUTINE(effectFactionModifier)
    REO_DECL_ROUTINE(effectForceBody)
    REO_DECL_ROUTINE(effectForceDrain)
    REO_DECL_ROUTINE(effectForceFizzle)
    REO_DECL_ROUTINE(effectForceJump)
    REO_DECL_ROUTINE(effectForcePushed)
    REO_DECL_ROUTINE(effectForcePushTargeted)
    REO_DECL_ROUTINE(effectForceResistanceDecrease)
    REO_DECL_ROUTINE(effectForceResistanceIncrease)
    REO_DECL_ROUTINE(effectForceResisted)
    REO_DECL_ROUTINE(effectForceShield)
    REO_DECL_ROUTINE(effectForceSight)
    REO_DECL_ROUTINE(effectFPRegenModifier)
    REO_DECL_ROUTINE(effectFrightened)
    REO_DECL_ROUTINE(effectFury)
    REO_DECL_ROUTINE(effectHaste)
    REO_DECL_ROUTINE(effectHeal)
    REO_DECL_ROUTINE(effectHealForcePoints)
    REO_DECL_ROUTINE(effectHitPointChangeWhenDying)
    REO_DECL_ROUTINE(effectHorrified)
    REO_DECL_ROUTINE(effectImmunity)
    REO_DECL_ROUTINE(effectInvisibility)
    REO_DECL_ROUTINE(effectKnockdown)
    REO_DECL_ROUTINE(effectLightsaberThrow)
    REO_DECL_ROUTINE(effectLinkEffects)
    REO_DECL_ROUTINE(effectMindTrick)
    REO_DECL_ROUTINE(effectMissChance)
    REO_DECL_ROUTINE(effectModifyAttacks)
    REO_DECL_ROUTINE(effectMovementSpeedDecrease)
    REO_DECL_ROUTINE(effectMovementSpeedIncrease)
    REO_DECL_ROUTINE(effectParalyze)
    REO_DECL_ROUTINE(effectPoison)
    REO_DECL_ROUTINE(effectPsychicStatic)
    REO_DECL_ROUTINE(effectRegenerate)
    REO_DECL_ROUTINE(effectResurrection)
    REO_DECL_ROUTINE(effectSavingThrowDecrease)
    REO_DECL_ROUTINE(effectSavingThrowIncrease)
    REO_DECL_ROUTINE(effectSeeInvisible)
    REO_DECL_ROUTINE(effectSkillDecrease)
    REO_DECL_ROUTINE(effectSkillIncrease)
    REO_DECL_ROUTINE(effectSleep)
    REO_DECL_ROUTINE(effectSpellImmunity)
    REO_DECL_ROUTINE(effectSpellLevelAbsorption)
    REO_DECL_ROUTINE(effectStunned)
    REO_DECL_ROUTINE(effectTemporaryForcePoints)
    REO_DECL_ROUTINE(effectTemporaryHitpoints)
    REO_DECL_ROUTINE(effectTimeStop)
    REO_DECL_ROUTINE(effectTrueSeeing)
    REO_DECL_ROUTINE(effectVisualEffect)
    REO_DECL_ROUTINE(effectVPRegenModifier)
    REO_DECL_ROUTINE(effectWhirlWind)
    REO_DECL_ROUTINE(eventUserDefined)
    REO_DECL_ROUTINE(executeScript)
    REO_DECL_ROUTINE(fabs)
    REO_DECL_ROUTINE(faceObjectAwayFromObject)
    REO_DECL_ROUTINE(feetToMeters)
    REO_DECL_ROUTINE(findSubString)
    REO_DECL_ROUTINE(floatToInt)
    REO_DECL_ROUTINE(floatToString)
    REO_DECL_ROUTINE(getAbilityScore)
    REO_DECL_ROUTINE(getArea)
    REO_DECL_ROUTINE(getAreaUnescapable)
    REO_DECL_ROUTINE(getAttackTarget)
    REO_DECL_ROUTINE(getAttemptedAttackTarget)
    REO_DECL_ROUTINE(getAttemptedSpellTarget)
    REO_DECL_ROUTINE(getBaseItemType)
    REO_DECL_ROUTINE(getClassByPosition)
    REO_DECL_ROUTINE(getCommandable)
    REO_DECL_ROUTINE(getCurrentAction)
    REO_DECL_ROUTINE(getCurrentHitPoints)
    REO_DECL_ROUTINE(getCurrentStealthXP)
    REO_DECL_ROUTINE(getDistanceBetween)
    REO_DECL_ROUTINE(getDistanceBetween2D)
    REO_DECL_ROUTINE(getDistanceBetweenLocations)
    REO_DECL_ROUTINE(getDistanceBetweenLocations2D)
    REO_DECL_ROUTINE(getDistanceToObject)
    REO_DECL_ROUTINE(getDistanceToObject2D)
    REO_DECL_ROUTINE(getEnteringObject)
    REO_DECL_ROUTINE(getExitingObject)
    REO_DECL_ROUTINE(getFacing)
    REO_DECL_ROUTINE(getFacingFromLocation)
    REO_DECL_ROUTINE(getFactionEqual)
    REO_DECL_ROUTINE(getFirstItemInInventory)
    REO_DECL_ROUTINE(getFirstPC)
    REO_DECL_ROUTINE(getGender)
    REO_DECL_ROUTINE(getGlobalBoolean)
    REO_DECL_ROUTINE(getGlobalLocation)
    REO_DECL_ROUTINE(getGlobalNumber)
    REO_DECL_ROUTINE(getGlobalString)
    REO_DECL_ROUTINE(getHasSkill)
    REO_DECL_ROUTINE(getHasSpell)
    REO_DECL_ROUTINE(getHitDice)
    REO_DECL_ROUTINE(getIdentified)
    REO_DECL_ROUTINE(getIsDawn)
    REO_DECL_ROUTINE(getIsDay)
    REO_DECL_ROUTINE(getIsDead)
    REO_DECL_ROUTINE(getIsDebilitated)
    REO_DECL_ROUTINE(getIsDusk)
    REO_DECL_ROUTINE(getIsEnemy)
    REO_DECL_ROUTINE(getIsFriend)
    REO_DECL_ROUTINE(getIsInCombat)
    REO_DECL_ROUTINE(getIsNeutral)
    REO_DECL_ROUTINE(getIsNight)
    REO_DECL_ROUTINE(getIsObjectValid)
    REO_DECL_ROUTINE(getIsOpen)
    REO_DECL_ROUTINE(getIsPC)
    REO_DECL_ROUTINE(getItemInSlot)
    REO_DECL_ROUTINE(getItemPossessedBy)
    REO_DECL_ROUTINE(getItemStackSize)
    REO_DECL_ROUTINE(getLastAttackAction)
    REO_DECL_ROUTINE(getLastHostileTarget)
    REO_DECL_ROUTINE(getLastOpenedBy)
    REO_DECL_ROUTINE(getLastPerceived)
    REO_DECL_ROUTINE(getLastPerceptionHeard)
    REO_DECL_ROUTINE(getLastPerceptionInaudible)
    REO_DECL_ROUTINE(getLastPerceptionSeen)
    REO_DECL_ROUTINE(getLastPerceptionVanished)
    REO_DECL_ROUTINE(getLevelByClass)
    REO_DECL_ROUTINE(getLevelByPosition)
    REO_DECL_ROUTINE(getLoadFromSaveGame)
    REO_DECL_ROUTINE(getLocalBoolean)
    REO_DECL_ROUTINE(getLocalNumber)
    REO_DECL_ROUTINE(getLocation)
    REO_DECL_ROUTINE(getLocked)
    REO_DECL_ROUTINE(getMaxHitPoints)
    REO_DECL_ROUTINE(getMaxStealthXP)
    REO_DECL_ROUTINE(getMinOneHP)
    REO_DECL_ROUTINE(getModule)
    REO_DECL_ROUTINE(getName)
    REO_DECL_ROUTINE(getNearestCreature)
    REO_DECL_ROUTINE(getNearestCreatureToLocation)
    REO_DECL_ROUTINE(getNearestObject)
    REO_DECL_ROUTINE(getNearestObjectByTag)
    REO_DECL_ROUTINE(getNearestObjectToLocation)
    REO_DECL_ROUTINE(getNextItemInInventory)
    REO_DECL_ROUTINE(getNPCAIStyle)
    REO_DECL_ROUTINE(getObjectByTag)
    REO_DECL_ROUTINE(getObjectHeard)
    REO_DECL_ROUTINE(getObjectSeen)
    REO_DECL_ROUTINE(getObjectType)
    REO_DECL_ROUTINE(getPartyLeader)
    REO_DECL_ROUTINE(getPartyMemberByIndex)
    REO_DECL_ROUTINE(getPartyMemberCount)
    REO_DECL_ROUTINE(getPCSpeaker)
    REO_DECL_ROUTINE(getPlayerRestrictMode)
    REO_DECL_ROUTINE(getPlotFlag)
    REO_DECL_ROUTINE(getPosition)
    REO_DECL_ROUTINE(getPositionFromLocation)
    REO_DECL_ROUTINE(getRacialType)
    REO_DECL_ROUTINE(getRunScriptVar)
    REO_DECL_ROUTINE(getSkillRank)
    REO_DECL_ROUTINE(getSoloMode)
    REO_DECL_ROUTINE(getSpellTarget)
    REO_DECL_ROUTINE(getStandardFaction)
    REO_DECL_ROUTINE(getStartingLocation)
    REO_DECL_ROUTINE(getStealthXPDecrement)
    REO_DECL_ROUTINE(getStealthXPEnabled)
    REO_DECL_ROUTINE(getStringByStrRef)
    REO_DECL_ROUTINE(getStringLeft)
    REO_DECL_ROUTINE(getStringLength)
    REO_DECL_ROUTINE(getStringLowerCase)
    REO_DECL_ROUTINE(getStringRight)
    REO_DECL_ROUTINE(getStringUpperCase)
    REO_DECL_ROUTINE(getSubRace)
    REO_DECL_ROUTINE(getSubString)
    REO_DECL_ROUTINE(getTag)
    REO_DECL_ROUTINE(getTimeHour)
    REO_DECL_ROUTINE(getTimeMillisecond)
    REO_DECL_ROUTINE(getTimeMinute)
    REO_DECL_ROUTINE(getTimeSecond)
    REO_DECL_ROUTINE(getUserActionsPending)
    REO_DECL_ROUTINE(getUserDefinedEventNumber)
    REO_DECL_ROUTINE(getWaypointByTag)
    REO_DECL_ROUTINE(getXP)
    REO_DECL_ROUTINE(giveXPToCreature)
    REO_DECL_ROUTINE(hoursToSeconds)
    REO_DECL_ROUTINE(insertString)
    REO_DECL_ROUTINE(intToFloat)
    REO_DECL_ROUTINE(intToHexString)
    REO_DECL_ROUTINE(intToString)
    REO_DECL_ROUTINE(isAvailableCreature)
    REO_DECL_ROUTINE(isNPCPartyMember)
    REO_DECL_ROUTINE(isObjectPartyMember)
    REO_DECL_ROUTINE(jumpToLocation)
    REO_DECL_ROUTINE(jumpToObject)
    REO_DECL_ROUTINE(location)
    REO_DECL_ROUTINE(log)
    REO_DECL_ROUTINE(objectToString)
    REO_DECL_ROUTINE(playAnimation)
    REO_DECL_ROUTINE(playMovie)
    REO_DECL_ROUTINE(pow)
    REO_DECL_ROUTINE(printFloat)
    REO_DECL_ROUTINE(printInteger)
    REO_DECL_ROUTINE(printObject)
    REO_DECL_ROUTINE(printString)
    REO_DECL_ROUTINE(printVector)
    REO_DECL_ROUTINE(random)
    REO_DECL_ROUTINE(removeAvailableNPC)
    REO_DECL_ROUTINE(removePartyMember)
    REO_DECL_ROUTINE(roundsToSeconds)
    REO_DECL_ROUTINE(setAreaUnescapable)
    REO_DECL_ROUTINE(setCommandable)
    REO_DECL_ROUTINE(setCurrentStealthXP)
    REO_DECL_ROUTINE(setFacing)
    REO_DECL_ROUTINE(setFacingPoint)
    REO_DECL_ROUTINE(setGlobalBoolean)
    REO_DECL_ROUTINE(setGlobalLocation)
    REO_DECL_ROUTINE(setGlobalNumber)
    REO_DECL_ROUTINE(setGlobalString)
    REO_DECL_ROUTINE(setIdentified)
    REO_DECL_ROUTINE(setItemStackSize)
    REO_DECL_ROUTINE(setLocalBoolean)
    REO_DECL_ROUTINE(setLocalNumber)
    REO_DECL_ROUTINE(setLocked)
    REO_DECL_ROUTINE(setMaxHitPoints)
    REO_DECL_ROUTINE(setMaxStealthXP)
    REO_DECL_ROUTINE(setMinOneHP)
    REO_DECL_ROUTINE(setNPCAIStyle)
    REO_DECL_ROUTINE(setPartyLeader)
    REO_DECL_ROUTINE(setPlayerRestrictMode)
    REO_DECL_ROUTINE(setPlotFlag)
    REO_DECL_ROUTINE(setSoloMode)
    REO_DECL_ROUTINE(setStealthXPDecrement)
    REO_DECL_ROUTINE(setStealthXPEnabled)
    REO_DECL_ROUTINE(setTime)
    REO_DECL_ROUTINE(setXP)
    REO_DECL_ROUTINE(shipBuild)
    REO_DECL_ROUTINE(showPartySelectionGUI)
    REO_DECL_ROUTINE(signalEvent)
    REO_DECL_ROUTINE(sin)
    REO_DECL_ROUTINE(soundObjectPlay)
    REO_DECL_ROUTINE(soundObjectStop)
    REO_DECL_ROUTINE(sqrt)
    REO_DECL_ROUTINE(startNewModule)
    REO_DECL_ROUTINE(stringToFloat)
    REO_DECL_ROUTINE(stringToInt)
    REO_DECL_ROUTINE(tan)
    REO_DECL_ROUTINE(turnsToSeconds)
    REO_DECL_ROUTINE(vectorCreate)
    REO_DECL_ROUTINE(vectorMagnitude)
    REO_DECL_ROUTINE(vectorNormalize)
    REO_DECL_ROUTINE(yardsToMeters)

    // END Routine implementations
};

} // namespace game

} // namespace reone
