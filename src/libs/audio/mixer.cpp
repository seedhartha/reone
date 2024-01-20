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

#include "reone/audio/mixer.h"

namespace reone {

namespace audio {

void AudioMixer::render() {
    for (auto it = _sources.begin(); it != _sources.end();) {
        auto &source = *it;
        source->render();
        if (!source->isPlaying()) {
            it = _sources.erase(it);
        } else {
            ++it;
        }
    }
}

std::shared_ptr<AudioSource> AudioMixer::play(std::shared_ptr<AudioClip> clip,
                                              AudioType type,
                                              float gain,
                                              bool loop,
                                              std::optional<glm::vec3> position) {
    auto source = std::make_shared<AudioSource>(
        std::move(clip),
        gainByType(type, gain),
        loop,
        std::move(position));
    source->init();
    source->play();
    _sources.push_back(source);
    return source;
}

float AudioMixer::gainByType(AudioType type, float gain) const {
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
