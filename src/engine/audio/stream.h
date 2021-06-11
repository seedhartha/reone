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

#include "../common/types.h"

#include "types.h"

namespace reone {

namespace audio {

class AudioStream : boost::noncopyable {
public:
    struct Frame {
        AudioFormat format { AudioFormat::Mono8 };
        int sampleRate { 0 };
        ByteArray samples;
    };

    void add(Frame &&frame);
    void fill(int frameIdx, uint32_t buffer);

    int getFrameCount() const;
    const Frame &getFrame(int index) const;

    float duration() const { return _duration; }

private:
    float _duration { 0 };
    std::vector<Frame> _frames;

    int getALAudioFormat(AudioFormat format) const;
};

} // namespace audio

} // namespace reone
