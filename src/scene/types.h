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

struct AnimationFlags {
    static constexpr int loop = 1;
    static constexpr int blend = 4; /**< blend previous animation into the next one */
    static constexpr int overlay = 8; /**< overlay next animation on top of the previous one */
    static constexpr int propagateHead = 0x10; /**< propagate animation to the head model, if any */
    static constexpr int syncLipAnim = 0x20; /**< animation must be synchronized with the lip animation */

    static constexpr int loopOverlay = loop | overlay;
    static constexpr int loopBlend = loop | blend;
};

} // namespace scene

} // namespace reone
