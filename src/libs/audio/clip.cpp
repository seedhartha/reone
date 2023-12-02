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

#include "reone/audio/clip.h"

namespace reone {

namespace audio {

void AudioClip::add(Frame &&frame) {
    _duration += frame.samples.size() / frame.stride() / static_cast<float>(frame.sampleRate);
    _frames.push_back(frame);
}

int AudioClip::getFrameCount() const {
    return static_cast<int>(_frames.size());
}

const AudioClip::Frame &AudioClip::getFrame(int index) const {
    return _frames[index];
}

} // namespace audio

} // namespace reone
