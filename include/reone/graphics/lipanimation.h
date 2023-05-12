/*
 * Copyright (c) 2020-2023 The reone project contributors
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

namespace graphics {

class LipAnimation {
public:
    struct Keyframe {
        float time {0.0f};
        uint8_t shape {0}; /**< an index into keyframes of the "talk" animation  */
    };

    LipAnimation(std::string name, float length, std::vector<Keyframe> keyframes) :
        _name(std::move(name)),
        _length(length),
        _keyframes(std::move(keyframes)) {
    }

    bool getKeyframes(float time, uint8_t &leftShape, uint8_t &rightShape, float &factor) const;

    const std::string &name() const { return _name; }
    float length() const { return _length; }
    const std::vector<Keyframe> &keyframes() const { return _keyframes; }

private:
    std::string _name;
    float _length;
    std::vector<Keyframe> _keyframes;
};

} // namespace graphics

} // namespace reone
