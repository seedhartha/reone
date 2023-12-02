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

#include "options.h"
#include "source.h"
#include "types.h"

namespace reone {

namespace audio {

class AudioClip;

class IAudioPlayer {
public:
    virtual ~IAudioPlayer() = default;

    virtual std::shared_ptr<AudioSource> play(std::shared_ptr<AudioClip> clip, AudioType type, bool loop = false, float gain = 1.0f, bool positional = false, glm::vec3 position = glm::vec3(0.0f)) = 0;
};

class AudioPlayer : public IAudioPlayer, boost::noncopyable {
public:
    AudioPlayer(AudioOptions &options) :
        _options(options) {
    }

    std::shared_ptr<AudioSource> play(std::shared_ptr<AudioClip> clip, AudioType type, bool loop = false, float gain = 1.0f, bool positional = false, glm::vec3 position = glm::vec3(0.0f)) override;

private:
    AudioOptions &_options;

    float getGain(AudioType type, float gain) const;
};

} // namespace audio

} // namespace reone
