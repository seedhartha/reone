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

#include <memory>
#include <string>
#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "../audio/types.h"
#include "../net/types.h"
#include "../render/texture.h"
#include "../render/types.h"
#include "../resource/types.h"

#include "rp/types.h"

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
    SingleSaber = 2,
    TwoHandedSaber = 3,
    SingleBlaster = 4,
    Rifle = 5,
    HeavyCarbine = 6
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

enum class AttackResult {
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

class CreatureBlueprint;

struct Options {
    std::string module;
    render::GraphicsOptions graphics;
    audio::AudioOptions audio;
    net::NetworkOptions network;
};

struct CreatureConfiguration {
    std::shared_ptr<CreatureBlueprint> blueprint;
    Gender gender { Gender::Male };
    ClassType clazz { ClassType::Soldier };
    int appearance { 0 };
    std::vector<std::string> equipment;

    bool operator==(const CreatureConfiguration &other) {
        return
            blueprint == other.blueprint &&
            gender == other.gender &&
            clazz == other.clazz &&
            appearance == other.appearance &&
            equipment == other.equipment;
    }
};

struct Portrait {
    std::string resRef;
    std::shared_ptr<render::Texture> image;
    int appearanceNumber { 0 };
    int appearanceS { 0 };
    int appearanceL { 0 };
};

} // namespace game

} // namespace reone
