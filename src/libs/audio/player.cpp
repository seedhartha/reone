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

#include "reone/audio/player.h"

namespace reone {

namespace audio {

std::shared_ptr<AudioSource> AudioPlayer::play(std::shared_ptr<AudioBuffer> buffer, AudioType type, bool loop, float gain, bool positional, glm::vec3 position) {
    auto source = std::make_shared<AudioSource>(std::move(buffer), loop, getGain(type, gain), positional, std::move(position));
    source->init();
    source->play();
    return source;
}

float AudioPlayer::getGain(AudioType type, float gain) const {
    int volume;
    switch (type) {
    case AudioType::Music:
        volume = _options.musicVolume;
        break;
    case AudioType::Voice:
        volume = _options.voiceVolume;
        break;
    case AudioType::Sound:
        volume = _options.soundVolume;
        break;
    case AudioType::Movie:
        volume = _options.movieVolume;
        break;
    default:
        volume = 85.0f;
        break;
    }
    return gain * (volume / 100.0f);
}

} // namespace audio

} // namespace reone
