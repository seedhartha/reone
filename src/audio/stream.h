/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../common/types.h"

#include "types.h"

namespace reone {

namespace audio {

class AudioStream {
public:
    struct Frame {
        AudioFormat format { AudioFormat::Mono8 };
        int sampleRate { 0 };
        ByteArray samples;
    };

    AudioStream() = default;

    void add(Frame &&frame);
    void fill(int frameIdx, uint32_t buffer);

    int duration() const;
    int frameCount() const;
    const Frame &getFrame(int index) const;

private:
    float _duration { 0 };
    std::vector<Frame> _frames;

    AudioStream(const AudioStream &) = delete;
    AudioStream &operator=(const AudioStream &) = delete;

    int getALAudioFormat(AudioFormat format) const;
};

} // namespace audio

} // namespace reone
