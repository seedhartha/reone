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

#include <algorithm>
#include <vector>

#include "glm/common.hpp"
#include "glm/gtc/quaternion.hpp"

namespace reone {

namespace render {

template <class T>
struct MixInterpolator {
    static T interpolate(const T &left, const T &right, float factor) {
        return glm::mix(left, right, factor);
    }
};

struct SlerpInterpolator {
    static glm::quat interpolate(const glm::quat &left, const glm::quat &right, float factor) {
        return glm::slerp(left, right, factor);
    }
};

template <class V, class Interpolator = MixInterpolator<V>>
class AnimatedProperty {
public:
    int getNumKeyframes() const {
        return static_cast<int>(_keyframes.size());
    }

    bool getByTime(float time, V &value) const {
        if (_keyframes.empty()) return false;

        const std::pair<float, V> *frame1 = &_keyframes[0];
        const std::pair<float, V> *frame2 = &_keyframes[0];
        for (auto it = _keyframes.begin(); it != _keyframes.end(); ++it) {
            if (it->first >= time) {
                frame2 = &*it;
                if (it != _keyframes.begin()) {
                    frame1 = &*(it - 1);
                }
                break;
            }
        }

        float factor;
        if (frame1 == frame2) {
            factor = 0.0f;
        } else {
            factor = (time - frame1->first) / (frame2->first - frame1->first);
        }

        value = Interpolator::interpolate(frame1->second, frame2->second, factor);

        return true;
    }

    V getByKeyframe(int frame) const {
        return _keyframes[frame].second;
    }

    V getByKeyframeOrDefault(int frame, V defaultValue) const {
        return frame < static_cast<int>(_keyframes.size()) ?
            getByKeyframe(frame) :
            std::move(defaultValue);
    }

    void addKeyframe(float time, V value) {
        _keyframes.push_back(std::make_pair(time, value));
    }

    void update() {
        std::sort(_keyframes.begin(), _keyframes.end(), [](auto &left, auto &right) {
            return left.first < right.first;
        });
    }

private:
    std::vector<std::pair<float, V>> _keyframes;
};

} // namespace render

} // namespace reone
