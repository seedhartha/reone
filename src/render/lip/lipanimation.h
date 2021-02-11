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

#include <cstdint>
#include <vector>

namespace reone {

namespace render {

class LipAnimation {
public:
    struct Keyframe {
        float time { 0.0f };
        uint8_t shape { 0 }; /**< an index into the keyframes of the "talk" animation  */
    };

    LipAnimation(float length, std::vector<Keyframe> keyframes);

    bool getKeyframes(float time, uint8_t &leftShape, uint8_t &rightShape, float &interpolant) const;

    float length() const { return _length; }

private:
    float _length { 0.0f };
    std::vector<Keyframe> _keyframes;
};

} // namespace render

} // namespace reone
