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

namespace audio {

class SoundHandle : boost::noncopyable {
public:
    enum class State {
        NotInited,
        Playing,
        Stopped
    };

    SoundHandle(float duration, const glm::vec3 &position);

    void stop();
    void resetPositionDirty();

    bool isNotInited() const;
    bool isStopped() const;
    bool isPositionDirty() const { return _positionDirty; }

    float duration() const { return _duration; }
    glm::vec3 position() const { return _position; }

    void setState(State state);
    void setPosition(const glm::vec3 &position);

private:
    std::atomic<State> _state { State::NotInited };
    float _duration { 0.0f };
    std::atomic<glm::vec3> _position;
    std::atomic_bool _positionDirty { false };
};

} // namespace audio

} // namespace reone
