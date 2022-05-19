/*
 * Copyright (c) 2020-2022 The reone project contributors
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

/**
 * @see AnimationFlags
 */
struct AnimationProperties {
    int flags {0};
    float speed {1.0f};
    float scale {0.0f};     /**< animation scale; 0.0 to use the models scale */
    float duration {-1.0f}; /**< duration in seconds; -1.0 is infinity */

    bool operator==(const AnimationProperties &other) const {
        return flags == other.flags &&
               speed == other.speed &&
               scale == other.scale;
    }

    static AnimationProperties fromFlags(int flags) {
        AnimationProperties properties;
        properties.flags = flags;
        return std::move(properties);
    }
};

} // namespace scene

} // namespace reone
