/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../object/object.h"

namespace reone {

namespace game {

class Game;

class Routines : public script::IRoutineProvider {
public:
    static Routines &instance();

    void init(resource::GameVersion version, Game *game);
    void deinit();

    const script::Routine &get(int index) override;

private:
    Game *_game { nullptr };
    std::vector<script::Routine> _routines;

    Routines() = default;
    Routines(const Routines &) = delete;
    ~Routines();

    Routines &operator=(const Routines &) = delete;

    void add(const std::string &name, script::VariableType retType, const std::vector<script::VariableType> &argTypes);

    void add(
        const std::string &name,
        script::VariableType retType,
        const std::vector<script::VariableType> &argTypes,
        const std::function<script::Variable(const std::vector<script::Variable>&, script::ExecutionContext &ctx)> &fn);

    void addKotorRoutines();
    void addTslRoutines();

    std::shared_ptr<Object> getObjectById(uint32_t id, const script::ExecutionContext &ctx) const;

    // Common

    script::Variable d2(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable d3(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable d4(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable d6(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable d8(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable d10(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable d12(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable d20(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable d100(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    script::Variable feetToMeters(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable floatToInt(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable floatToString(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable hoursToSeconds(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable intToFloat(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable intToHexString(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable intToString(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable roundsToSeconds(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable stringToFloat(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable stringToInt(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable turnsToSeconds(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable yardsToMeters(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    script::Variable getStringLength(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getStringUpperCase(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getStringLowerCase(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getStringRight(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getStringLeft(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable insertString(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getSubString(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable findSubString(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    script::Variable executeScript(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getLoadFromSaveGame(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getRunScriptVar(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable random(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable shipBuild(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    // END Common

    // Objects

    script::Variable createItemOnObject(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable destroyObject(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getArea(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getEnteringObject(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getIsObjectValid(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getItemInSlot(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getLocked(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getObjectByTag(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getWaypointByTag(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable setLocked(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    // END Objects

    // Globals/locals

    script::Variable setLocalNumber(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable setLocalBoolean(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable setGlobalNumber(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable setGlobalBoolean(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getLocalNumber(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getLocalBoolean(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getGlobalNumber(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getGlobalBoolean(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    // END Globals/locals

    // Events

    script::Variable eventUserDefined(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable signalEvent(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getUserDefinedEventNumber(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    // END Events

    // Party

    script::Variable addAvailableNPCByTemplate(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getFirstPC(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getIsPC(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getPartyMemberByIndex(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getPCSpeaker(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable isAvailableCreature(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable isNPCPartyMember(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable isObjectPartyMember(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable setPartyLeader(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable showPartySelectionGUI(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    // END Party

    // Actions

    script::Variable actionCloseDoor(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable actionDoCommand(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable actionMoveToObject(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable actionOpenDoor(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable actionPauseConversation(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable actionResumeConversation(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable actionStartConversation(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable assignCommand(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable clearAllActions(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable delayCommand(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    // END Actions

    // Role-playing

    script::Variable getClassByPosition(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getGender(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getHasSkill(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getHitDice(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getLevelByClass(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    // END Role-playing

    // Math

    script::Variable abs(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable acos(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable asin(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable atan(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable cos(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable fabs(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable log(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable pow(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable sin(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable sqrt(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable tan(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);

    // END Math
};

} // namespace game

} // namespace reone
