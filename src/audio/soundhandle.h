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

namespace reone {

namespace audio {

class SoundHandle {
public:
    enum class State {
        NotInited,
        Playing,
        Stopped
    };

    SoundHandle() = default;

    void stop();

    bool isNotInited() const;
    bool isStopped() const;

    int duration() const;

    void setState(State state);
    void setDuration(int duration);

private:
    std::atomic<State> _state { State::NotInited };
    int _duration { 0 };

    SoundHandle(const SoundHandle &) = delete;
    SoundHandle &operator=(const SoundHandle &) = delete;
};

} // namespace audio

} // namespace reone
