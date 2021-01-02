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

namespace reone {

namespace game {

enum class ActionType {
    MoveToPoint = 0,
    PickUpItem = 1,
    DropItem = 2,
    AttackObject = 3,
    CastSpell = 4,
    OpenDoor = 5,
    CloseDoor = 6,
    DialogObject = 7,
    DisableTrap = 8,
    RecoverTrap = 9,
    FlagTrap = 10,
    ExamineTrap = 11,
    SetTrap = 12,
    OpenLock = 13,
    Lock = 14,
    UseObject = 15,
    AnimalEmpathy = 16,
    Rest = 17,
    Taunt = 18,
    ItemCastSpell = 19,
    CounterSpell = 31,
    Heal = 33,
    PickPocket = 34,
    Follow = 35,
    Wait = 36,
    Sit = 37,
    FollowLeader = 38,
    FollowOwner = 43,

    DoCommand = 0x1000,
    StartConversation = 0x1001,
    PauseConversation = 0x1002,
    ResumeConversation = 0x1003,
    MoveToObject = 0x1004,
    OpenContainer = 0x1005,
    JumpToObject = 0x1006,
    JumpToLocation = 0x1007,
    RandomWalk = 0x1008,
    MoveToLocation = 0x1009,
    MoveAwayFromObject = 0x100a,
    EquipItem = 0x100b,
    UnequipItem = 0x100c,
    SpeakString = 0x100d,
    PlayAnimation = 0x100e,
    CastSpellAtObject = 0x100f,
    GiveItem = 0x1010,
    TakeItem = 0x1011,
    ForceFollowObject = 0x1012,
    CastSpellAtLocation = 0x1013,
    SpeakStringByStrRef = 0x1014,
    UseFeat = 0x1015,
    UseSkill = 0x1016,
    UseTalentOnObject = 0x1017,
    UseTalentAtLocation = 0x1018,
    InteractObject = 0x1019,
    MoveAwayFromLocation = 0x101a,
    SurrenderToEnemies = 0x101b,
    EquipMostDamagingMelee = 0x101c,
    EquipMostDamagingRanged = 0x101d,
    EquipMostEffectiveArmor = 0x101e,
    UnlockObject = 0x101f,
    LockObject = 0x1020,
    CastFakeSpellAtObject = 0x1021,
    CastFakeSpellAtLocation = 0x1022,
    BarkString = 0x1023,
    SwitchWeapons = 0x1024,

    Invalid = 0xffff,
    QueueEmpty = 0xfffe
};

class Action {
public:
    Action(ActionType type);
    virtual ~Action() = default;

    void complete();

    ActionType type() const;
    bool isCompleted() const;

protected:
    ActionType _type { ActionType::QueueEmpty };
    bool _completed { false };

private:
    Action(const Action &) = delete;
    Action &operator=(const Action &) = delete;
};

} // namespace game

} // namespace reone
