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

    void add(
        const std::string &name,
        script::VariableType retType,
        const VariableTypesList &argTypes,
        const std::function<script::Variable(const VariablesList &, script::ExecutionContext &ctx)> &fn);

    void addKotorRoutines();
    void addTslRoutines();

    bool getBool(const VariablesList &args, int index, bool defValue = false) const;
    int getInt(const VariablesList &args, int index, int defValue = 0) const;
    float getFloat(const VariablesList &args, int index, float defValue = 0.0f) const;
    std::string getString(const VariablesList &args, int index, std::string defValue = "") const;
    glm::vec3 getVector(const VariablesList &args, int index, glm::vec3 defValue = glm::vec3(0.0f)) const;
    std::shared_ptr<Object> getCaller(script::ExecutionContext &ctx) const;
    std::shared_ptr<SpatialObject> getCallerAsSpatial(script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getTriggerrer(script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getObject(const VariablesList &args, int index) const;
    std::shared_ptr<Object> getObjectOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<SpatialObject> getSpatialObject(const VariablesList &args, int index) const;
    std::shared_ptr<SpatialObject> getSpatialObjectOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Creature> getCreature(const VariablesList &args, int index) const;
    std::shared_ptr<Creature> getCreatureOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Door> getDoor(const VariablesList &args, int index) const;
    std::shared_ptr<Sound> getSound(const VariablesList &args, int index) const;
    std::shared_ptr<Location> getLocationEngineType(const VariablesList &args, int index) const;
    std::shared_ptr<Event> getEvent(const VariablesList &args, int index) const;
    const script::ExecutionContext &getAction(const VariablesList &args, int index) const;

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

    // END Common

    // Objects

    script::Variable createItemOnObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable destroyObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getArea(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getDistanceBetween(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getDistanceToObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getDistanceToObject2D(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getEnteringObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getExitingObject(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getFacing(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getFirstItemInInventory(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getIsObjectValid(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getItemInSlot(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLocked(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getModule(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getNextItemInInventory(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getObjectByTag(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getPosition(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getTag(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getWaypointByTag(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setLocked(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable soundObjectPlay(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable soundObjectStop(const VariablesList &args, script::ExecutionContext &ctx);

    // END Objects

    // Engine types

    script::Variable getFacingFromLocation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLocation(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getPositionFromLocation(const VariablesList &args, script::ExecutionContext &ctx);
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

    script::Variable getClassByPosition(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getCurrentHitPoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getGender(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getHasSkill(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getHitDice(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getLevelByClass(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getMaxHitPoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable getMinOneHP(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setMaxHitPoints(const VariablesList &args, script::ExecutionContext &ctx);
    script::Variable setMinOneHP(const VariablesList &args, script::ExecutionContext &ctx);

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
};

} // namespace game

} // namespace reone
