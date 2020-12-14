/*
 * Copyright (c) 2020 The reone project contributors
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
#include <string>
#include <vector>

#include "../../resource/types.h"
#include "../../script/routine.h"
#include "../../script/types.h"
#include "../../script/variable.h"

namespace reone {

namespace game {

class Creature;
class Door;
class Event;
class Game;
class Item;
class Location;
class Object;
class Sound;
class SpatialObject;

class Routines : public script::IRoutineProvider {
public:
    static Routines &instance();

    void init(resource::GameVersion version, Game *game);
    void deinit();

    const script::Routine &get(int index) override;

private:
    typedef std::vector<script::VariableType> VariableTypesList;
    typedef std::vector<script::Variable> VariablesList;

    Game *_game { nullptr };
    std::vector<script::Routine> _routines;

    Routines() = default;
    Routines(const Routines &) = delete;
    ~Routines();

    Routines &operator=(const Routines &) = delete;

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

    bool getBool(const VariablesList &args, int index, bool defValue = false) const;
    const script::ExecutionContext &getAction(const VariablesList &args, int index) const;
    float getFloat(const VariablesList &args, int index, float defValue = 0.0f) const;
    glm::vec3 getVector(const VariablesList &args, int index, glm::vec3 defValue = glm::vec3(0.0f)) const;
    int getInt(const VariablesList &args, int index, int defValue = 0) const;
    std::shared_ptr<Creature> getCreature(const VariablesList &args, int index) const;
    std::shared_ptr<Creature> getCreatureOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Door> getDoor(const VariablesList &args, int index) const;
    std::shared_ptr<Event> getEvent(const VariablesList &args, int index) const;
    std::shared_ptr<Item> getItem(const VariablesList &args, int index) const;
    std::shared_ptr<Location> getLocationEngineType(const VariablesList &args, int index) const;
    std::shared_ptr<Object> getCaller(script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getObject(const VariablesList &args, int index) const;
    std::shared_ptr<Object> getObjectOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getTriggerrer(script::ExecutionContext &ctx) const;
    std::shared_ptr<Sound> getSound(const VariablesList &args, int index) const;
    std::shared_ptr<SpatialObject> getCallerAsSpatial(script::ExecutionContext &ctx) const;
    std::shared_ptr<SpatialObject> getSpatialObject(const VariablesList &args, int index) const;
    std::shared_ptr<SpatialObject> getSpatialObjectOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::string getString(const VariablesList &args, int index, std::string defValue = "") const;

    // Common

    script::Variable d2(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable d3(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable d4(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable d6(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable d8(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable d10(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable d12(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable d20(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable d100(const VariablesList &args, script::ExecutionContext &ctx);

    script::Variable feetToMeters(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable floatToInt(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable floatToString(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable hoursToSeconds(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable intToFloat(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable intToHexString(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable intToString(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable roundsToSeconds(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable stringToFloat(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable stringToInt(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable turnsToSeconds(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable yardsToMeters(const VariablesList &args, script::ExecutionContext &ctx);

    script::Variable getStringLength(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getStringUpperCase(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getStringLowerCase(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getStringRight(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getStringLeft(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable insertString(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getSubString(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable findSubString(const VariablesList &args, script::ExecutionContext &ctx);

    script::Variable executeScript(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLoadFromSaveGame(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getRunScriptVar(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getStringByStrRef(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable playMovie(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable random(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable shipBuild(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable startNewModule(const VariablesList &args, script::ExecutionContext &ctx);

    script::Variable printString(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable printFloat(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable printInteger(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable printObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable printVector(const VariablesList &args, script::ExecutionContext &ctx);

    // END Common

    // Objects

    script::Variable createItemOnObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable destroyObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable faceObjectAwayFromObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getArea(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getDistanceBetween(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getDistanceBetween2D(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getDistanceToObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getDistanceToObject2D(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getEnteringObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getExitingObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getFacing(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getFirstItemInInventory(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getIsDead(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getIsInCombat(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getIsObjectValid(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getIsOpen(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getItemInSlot(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getItemStackSize(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLocked(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getModule(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getName(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getNextItemInInventory(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getObjectByTag(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getObjectType(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getPlotFlag(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getPosition(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getTag(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getWaypointByTag(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getXP(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable giveXPToCreature(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setFacing(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setFacingPoint(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setItemStackSize(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setLocked(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setPlotFlag(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setXP(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable soundObjectPlay(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable soundObjectStop(const VariablesList &args, script::ExecutionContext &ctx);

    // END Objects

    // Engine types

    script::Variable getDistanceBetweenLocations(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getDistanceBetweenLocations2D(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getFacingFromLocation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLocation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getPositionFromLocation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getStartingLocation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable location(const VariablesList &args, script::ExecutionContext &ctx);

    // END Engine types

    // Globals/locals

    script::Variable getGlobalBoolean(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getGlobalNumber(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getGlobalString(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLocalBoolean(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLocalNumber(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setGlobalBoolean(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setGlobalNumber(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setGlobalString(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setLocalBoolean(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setLocalNumber(const VariablesList &args, script::ExecutionContext &ctx);

    // END Globals/locals

    // Events

    script::Variable eventUserDefined(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable signalEvent(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getUserDefinedEventNumber(const VariablesList &args, script::ExecutionContext &ctx);

    // END Events

    // Party

    script::Variable addAvailableNPCByTemplate(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable addPartyMember(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getFirstPC(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getIsPC(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getPartyMemberByIndex(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getPartyMemberCount(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getPCSpeaker(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable isAvailableCreature(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable isNPCPartyMember(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable isObjectPartyMember(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable removePartyMember(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setPartyLeader(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable showPartySelectionGUI(const VariablesList &args, script::ExecutionContext &ctx);

    // END Party

    // Actions

    script::Variable actionCloseDoor(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionDoCommand(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionForceMoveToLocation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionForceMoveToObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionJumpToLocation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionJumpToObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionMoveToObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionOpenDoor(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionPauseConversation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionResumeConversation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable actionStartConversation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable assignCommand(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable clearAllActions(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable delayCommand(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable jumpToLocation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable jumpToObject(const VariablesList &args, script::ExecutionContext &ctx);

    // END Actions

    // Role-playing

    script::Variable getAbilityScore(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getClassByPosition(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getCurrentHitPoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getGender(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getHasSkill(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getHitDice(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLevelByClass(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLevelByPosition(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getMaxHitPoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getMinOneHP(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getSkillRank(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setMaxHitPoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setMinOneHP(const VariablesList &args, script::ExecutionContext &ctx);

    script::Variable changeFaction(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable changeToStandardFaction(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getFactionEqual(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getIsEnemy(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getIsFriend(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getIsNeutral(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getStandardFaction(const VariablesList &args, script::ExecutionContext &ctx);

    // END Role-playing

    // Math

    script::Variable abs(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable acos(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable asin(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable atan(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable cos(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable fabs(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable log(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable pow(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable sin(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable sqrt(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable tan(const VariablesList &args, script::ExecutionContext &ctx);

    script::Variable vectorCreate(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable vectorNormalize(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable vectorMagnitude(const VariablesList &args, script::ExecutionContext &ctx);

    // END Math

    // Effects

    script::Variable effectAssuredHit(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectHeal(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDamage(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectAbilityIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDamageResistance(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectResurrection(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectACIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectSavingThrowIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectAttackIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDamageReduction(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDamageIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectEntangle(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDeath(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectKnockdown(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectParalyze(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectSpellImmunity(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForceJump(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectSleep(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectTemporaryForcePoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectConfused(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectFrightened(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectChoke(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectStunned(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectRegenerate(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectMovementSpeedIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectAreaOfEffect(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectVisualEffect(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectLinkEffects(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectBeam(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForceResistanceIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectBodyFuel(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectPoison(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectAssuredDeflection(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForcePushTargeted(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectHaste(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectImmunity(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDamageImmunityIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectTemporaryHitpoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectSkillIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDamageForcePoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectHealForcePoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectHitPointChangeWhenDying(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDroidStun(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForcePushed(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForceResisted(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForceFizzle(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectAbilityDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectAttackDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDamageDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDamageImmunityDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectACDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectMovementSpeedDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectSavingThrowDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectSkillDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForceResistanceDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectInvisibility(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectConcealment(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForceShield(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDispelMagicAll(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDisguise(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectTrueSeeing(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectSeeInvisible(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectTimeStop(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectBlasterDeflectionIncrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectBlasterDeflectionDecrease(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectHorrified(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectSpellLevelAbsorption(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDispelMagicBest(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectMissChance(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectModifyAttacks(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDamageShield(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForceDrain(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectPsychicStatic(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectLightsaberThrow(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectWhirlWind(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectCutSceneHorrified(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectCutSceneParalyze(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectCutSceneStunned(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForceBody(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectFury(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectBlind(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectFPRegenModifier(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectVPRegenModifier(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectCrush(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDroidConfused(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectForceSight(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectMindTrick(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectFactionModifier(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable effectDroidScramble(const VariablesList &args, script::ExecutionContext &ctx);

    // END Effects
};

} // namespace game

} // namespace reone
