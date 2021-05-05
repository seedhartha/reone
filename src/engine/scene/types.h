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

namespace scene {

enum class SceneNodeType {
    Mesh,
    Model,
    ModelNode,
    Camera,
    Light,
    Emitter,
    Grass
};

enum class ModelUsage {
    GUI,
    Room,
    Creature,
    Placeable,
    Door,
    Equipment,
    Projectile,
    Other
};

struct AnimationFlags {
    static constexpr int loop = 1;
    static constexpr int blend = 2; /**< blend previous animation into the next one */
    static constexpr int overlay = 4; /**< overlay next animation on top of the previous one */
    static constexpr int propagateHead = 8; /**< propagate animation to the head model, if any */
    static constexpr int syncLipAnim = 0x10; /**< animation must be synchronized with the lip animation */

    static constexpr int loopOverlay = loop | overlay;
    static constexpr int loopBlend = loop | blend;
};

struct SceneNodeStateFlags {
    static constexpr int transform = 1;
    static constexpr int alpha = 2;
    static constexpr int selfIllum = 4;
    static constexpr int lightColor = 8;
    static constexpr int lightMultiplier = 0x10;
    static constexpr int lightRadius = 0x20;

    static constexpr int all = transform | alpha | selfIllum | lightColor | lightMultiplier | lightRadius;
};

} // namespace scene

} // namespace reone
