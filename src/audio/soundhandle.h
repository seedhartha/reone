/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <atomic>

#include "../common/vector3.h"

namespace reone {

namespace audio {

class SoundHandle {
public:
    enum class State {
        NotInited,
        Playing,
        Stopped
    };

    SoundHandle(Vector3 position);

    void stop();

    bool isNotInited() const;
    bool isStopped() const;

    int duration() const;
    Vector3 position() const;
    bool isPositionDirty() const;

    void setState(State state);
    void setDuration(int duration);
    void setPosition(Vector3 position);
    void setPositionDirty(bool dirty);

private:
    std::atomic<State> _state { State::NotInited };
    int _duration { 0 };
    std::atomic<Vector3> _position;
    std::atomic_bool _positionDirty { false };

    SoundHandle(const SoundHandle &) = delete;
    SoundHandle &operator=(const SoundHandle &) = delete;
};

} // namespace audio

} // namespace reone
