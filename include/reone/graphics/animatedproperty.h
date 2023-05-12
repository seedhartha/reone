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

template <class V, class Inter = MixInterpolator<V>>
class AnimatedProperty {
public:
    int getNumFrames() const {
        return static_cast<int>(_frames.size());
    }

    bool getByTime(float time, V &value) const {
        if (_frames.empty())
            return false;

        const std::pair<float, V> *frame1 = &_frames[0];
        const std::pair<float, V> *frame2 = &_frames[0];
        for (auto it = _frames.begin(); it != _frames.end(); ++it) {
            if (it->first >= time) {
                frame2 = &*it;
                if (it != _frames.begin()) {
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

        value = Inter::interpolate(frame1->second, frame2->second, factor);

        return true;
    }

    V getByFrame(int frame) const {
        return _frames[frame].second;
    }

    V getByFrameOrElse(int frame, V defaultValue) const {
        return frame < static_cast<int>(_frames.size()) ? getByFrame(frame) : std::move(defaultValue);
    }

    void addFrame(float time, V value) {
        _frames.push_back(std::make_pair(time, std::move(value)));
    }

    void update() {
        std::sort(_frames.begin(), _frames.end(), [](auto &left, auto &right) {
            return left.first < right.first;
        });
    }

private:
    std::vector<std::pair<float, V>> _frames;
};

} // namespace graphics

} // namespace reone
