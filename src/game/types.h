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

#include <memory>
#include <unordered_map>

#include "../audio/stream.h"

namespace reone {

namespace game {

constexpr int kNpcPlayer = -1;
constexpr int kEngineTypeInvalid = -1;
constexpr float kDefaultFollowDistance = 2.0f;

enum class CursorType {
    None,
    Default,
    Attack,
    Door,
    Talk,
    Pickup,
    DisableMine,
    RecoverMine
};

enum class ContextualAction {
    None,
    Unlock,
    Attack
};

enum class WeaponType {
    None = 0,
    Melee = 1,
    Ranged = 4
};

enum class WeaponWield {
    None = 0,
    StunBaton = 1,
    SingleSword = 2,
    DoubleBladedSword = 3,
    BlasterPistol = 4,
    BlasterRifle = 5,
    HeavyWeapon = 6
};

enum class CreatureWieldType {
    None = 0,
    StunBaton = 1,
    SingleSword = 2,
    DoubleBladedSword = 3,
    DualSwords = 4,
    BlasterPistol = 5,
    DualPistols = 6,
    BlasterRifle = 7,
    HandToHand = 8,
    HeavyWeapon = 9
};

enum class DamageType {
    Bludgeoning = 1,
    Piercing = 2,
    Slashing = 4,
    Physical = 7,
    Universal = 8,
    Acid = 16,
    Cold = 32,
    LightSide = 64,
    Electrical = 128,
    Fire = 256,
    DarkSide = 512,
    Sonic = 1024,
    Ion = 2048,
    Blaster = 4096
};

enum class AttackResultType {
    Invalid = 0,
    HitSuccessful = 1,
    CriticalHit = 2,
    AutomaticHit = 3,
    Miss = 4,
    AttackResisted = 5,
    AttackFailed = 6,
    Parried = 8,
    Deflected = 9
};

enum class CameraStyleType {
    Default,
    Combat
};

enum class DurationType {
    Instant = 0,
    Temporary = 1,
    Permanent = 2
};

enum class ConversationType {
    Cinematic = 0,
    Computer = 1
};

enum class ComputerType {
    Normal = 0,
    Rakatan = 1
};

enum class SoundSetEntry {
    BattleCry1 = 0,
    BattleCry2 = 1,
    BattleCry3 = 2,
    BattleCry4 = 3,
    BattleCry5 = 4,
    BattleCry6 = 5,
    Select1 = 6,
    Select2 = 7,
    Select3 = 8,
    AttackGrunt1 = 9,
    AttackGrunt2 = 10,
    AttackGrunt3 = 11,
    PainGrunt1 = 12,
    PainGrunt2 = 13,
    LowHealth = 14,
    Dead = 15,
    CriticalHit = 16,
    TargetImmune = 17,
    LayMine = 18,
    DisarmMine = 19,
    BeginStealth = 20,
    BeginSearch = 21,
    BeginUnlock = 22,
    UnlockFailed = 23,
    UnlockSuccess = 24,
    SeparatedFromParty = 25,
    RejoinParty = 26,
    Poisoned = 27
};

struct InventorySlot {
    static constexpr int head = 0;
    static constexpr int body = 1;
    static constexpr int hands = 3;
    static constexpr int rightWeapon = 4;
    static constexpr int leftWeapon = 5;
    static constexpr int leftArm = 7;
    static constexpr int rightArm = 8;
    static constexpr int implant = 9;
    static constexpr int belt = 10;
    static constexpr int cWeaponL = 14;
    static constexpr int cWeaponR = 15;
    static constexpr int cWeaponB = 16;
    static constexpr int cArmour = 17;
    static constexpr int rightWeapon2 = 18;
    static constexpr int leftWeapon2 = 19;
};

typedef std::unordered_map<SoundSetEntry, std::shared_ptr<audio::AudioStream>> SoundSet;

} // namespace game

} // namespace reone
