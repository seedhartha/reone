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

const int kNpcPlayer = -1;
const int kEngineTypeInvalid = -1;

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

enum InventorySlot {
    kInventorySlotHead = 0,
    kInventorySlotBody = 1,
    kInventorySlotHands = 3,
    kInventorySlotRightWeapon = 4,
    kInventorySlotLeftWeapon = 5,
    kInventorySlotLeftArm = 7,
    kInventorySlotRightArm = 8,
    kInventorySlotImplant = 9,
    kInventorySlotBelt = 10,
    kInventorySlotCWeaponL = 14,
    kInventorySlotCWeaponR = 15,
    kInventorySlotCWeaponB = 16,
    kInventorySlotCArmour = 17,
    kInventorySlotRightWeapon2 = 18,
    kInventorySlotLeftWeapon2 = 19
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

} // namespace game

} // namespace reone
