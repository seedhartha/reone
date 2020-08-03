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

#include "stream.h"

#include <stdexcept>
#include <string>

#include "AL/al.h"

namespace reone {

namespace audio {

void AudioStream::add(Frame &&frame) {
    _frames.push_back(frame);
}

void AudioStream::fill(int frameIdx, uint32_t buffer) {
    if (frameIdx >= _frames.size()) {
        throw std::out_of_range("Frame index out of range: " + std::to_string(frameIdx));
    }
    Frame &frame = _frames[frameIdx];
    alBufferData(buffer, getALAudioFormat(frame.format), &frame.samples[0], frame.samples.size(), frame.sampleRate);
}

int AudioStream::getALAudioFormat(AudioFormat format) const {
    switch (format) {
        case AudioFormat::Mono8:
            return AL_FORMAT_MONO8;
        case AudioFormat::Mono16:
            return AL_FORMAT_MONO16;
        case AudioFormat::Stereo8:
            return AL_FORMAT_STEREO8;
        case AudioFormat::Stereo16:
            return AL_FORMAT_STEREO16;
        default:
            throw std::logic_error("Unknown audio format: " + std::to_string(static_cast<int>(format)));
    }
}

int AudioStream::frameCount() const {
    return _frames.size();
}

const AudioStream::Frame &AudioStream::getFrame(int index) const {
    return _frames[index];
}

} // namespace audio

} // namespace reone
