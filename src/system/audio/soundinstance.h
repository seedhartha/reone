/*
 * Copyright © 2020 Vsevolod Kremianskii
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
#include <memory>

#include "stream.h"

namespace reone {

namespace audio {

class SoundInstance {
public:
    SoundInstance(const std::shared_ptr<AudioStream> &stream, bool loop, float gain);
    SoundInstance(SoundInstance &&) = default;
    ~SoundInstance();

    SoundInstance &operator=(SoundInstance &&) = default;

    void update();
    void stop();

    bool stopped() const;
    int duration() const;

private:
    enum class State {
        NotInited,
        Playing,
        Stopped
    };

    std::shared_ptr<AudioStream> _stream;
    bool _loop { false };
    float _gain { 0.0f };
    bool _multiframe { false };
    std::atomic<State> _state { State::NotInited };
    int _nextFrame { 0 };
    int _nextBuffer { 0 };
    uint32_t _source { 0 };
    std::vector<uint32_t> _buffers;

    SoundInstance(SoundInstance &) = delete;
    SoundInstance &operator=(SoundInstance &) = delete;

    void init();
    void deinit();
};

} // namespace audio

} // namespace reone
