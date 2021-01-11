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

#include <cstdint>

namespace reone {

namespace mp {

struct Ballistic {
    uint8_t type;
    uint32_t timestamp;
    float x;
    float y;
    float z;

};

struct BaseStatus { 
    virtual ~BaseStatus() = default; // for dynamic_cast
};

struct SpatialStatus : public BaseStatus {
    float x { 0.0f };
    float y { 0.0f };
    float z { 0.0f };
    float facing { 0.0f };
    uint32_t maxHitPoints { 0 };
    uint32_t currentHitPoints { 0 };

};

enum class AnimationState : uint8_t {
    Talking = 1,
    InCombat = 1 << 1,
    HasMovement = 1 << 2,
    Run = 1 << 3, // 0 means walk, 1 means run
    //SetBodyAnim = 1 << 4 // whether to explicitly set body anim, outdated
};

struct AnimInfo {
    std::string name;
    uint32_t flag { 0 };
    uint32_t speed { 0 };
    uint32_t timestamp { 0 };
};

struct CreatureStatus : public BaseStatus {
    float x { 0.0f };
    float y { 0.0f };
    float z { 0.0f };
    float facing { 0.0f };
    
    // 8 bits, [ _talking, _inCombat, hasMovement, run,
    //           _animFireForget, _animDirty
    //         ]
    //           Outdated: ~~_setBodyAnim~~ ]
    uint8_t animStates { 0 };

    // Now, the action to explicitly playAnimation will be
    // delivered as a list of commands (to minimize overhead)
    // CreatureStatus only captures the casual animation state
    //uint16_t animIndex { 0 }; // int rather than string
    // uint16_t animSpeed { 0 };
    //uint32_t animFlag { 0 };
    //uint32_t animTimestamp { 0 };

    // responsible for setting _dead
    uint32_t maxHitPoints { 0 };
    uint32_t currentHitPoints { 0 };

    // only the visible ones
    std::string equipmentHead;
    std::string equipmentBody;
    std::string equipmentLeftWeapon;
    std::string equipmentRightWeapon;
};

} // namespace net

} // namespace reone
