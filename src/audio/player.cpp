/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/audio/files.h"

using namespace std;

namespace reone {

namespace audio {

shared_ptr<AudioSource> AudioPlayer::play(const string &resRef, AudioType type, bool loop, float gain, bool positional, glm::vec3 position) {
    shared_ptr<AudioStream> stream(_audioFiles.get(resRef));
    if (!stream) {
        return nullptr;
    }
    return play(move(stream), type, loop, gain, positional, move(position));
}

shared_ptr<AudioSource> AudioPlayer::play(shared_ptr<AudioStream> stream, AudioType type, bool loop, float gain, bool positional, glm::vec3 position) {
    auto source = make_shared<AudioSource>(move(stream), loop, getGain(type, gain), positional, move(position));
    source->init();
    source->play();
    return move(source);
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
