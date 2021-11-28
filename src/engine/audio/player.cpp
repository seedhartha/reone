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

#include "player.h"

#include "../common/logutil.h"

#include "files.h"
#include "soundhandle.h"
#include "soundinstance.h"
#include "stream.h"

using namespace std;

namespace reone {

namespace audio {

static constexpr float kMaxPositionalSoundDistance = 16.0f;
static constexpr float kMaxPositionalSoundDistance2 = kMaxPositionalSoundDistance * kMaxPositionalSoundDistance;

void AudioPlayer::init() {
    _device = alcOpenDevice(nullptr);
    if (!_device) {
        throw runtime_error("Failed to open an OpenAL device");
    }
    _context = alcCreateContext(_device, nullptr);
    if (!_context) {
        throw runtime_error("Failed to create an OpenAL context");
    }
    alcMakeContextCurrent(_context);
}

void AudioPlayer::deinit() {
    if (_context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(_context);
        _context = nullptr;
    }
    if (_device) {
        alcCloseDevice(_device);
        _device = nullptr;
    }
}

void AudioPlayer::update(float dt) {
    // Remove stopped sounds
    auto maybeSounds = remove_if(
        _sounds.begin(), _sounds.end(),
        [](auto &sound) { return sound->handle()->isStopped(); });
    _sounds.erase(maybeSounds, _sounds.end());

    // Update listener position
    if (_listenerPositionDirty) {
        glm::vec3 position(_listenerPosition);
        alListener3f(AL_POSITION, position.x, position.y, position.z);
        _listenerPositionDirty = false;
    }

    // Update sounds, init them if not already
    for (auto &sound : _sounds) {
        if (sound->handle()->isNotInited()) {
            sound->init();
        }
        sound->update();
    }
}

shared_ptr<SoundHandle> AudioPlayer::play(const string &resRef, AudioType type, bool loop, float gain, bool positional, glm::vec3 position) {
    shared_ptr<AudioStream> stream(_audioFiles.get(resRef));
    if (!stream) {
        return nullptr;
    }
    auto sound = make_shared<SoundInstance>(stream, loop, getGain(type, gain), positional, move(position));
    enqueue(sound);
    return sound->handle();
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
        return 85.0f;
    }
    return gain * (volume / 100.0f);
}

void AudioPlayer::enqueue(const shared_ptr<SoundInstance> &sound) {
    _sounds.push_back(sound);
}

shared_ptr<SoundHandle> AudioPlayer::play(const shared_ptr<AudioStream> &stream, AudioType type, bool loop, float gain, bool positional, glm::vec3 position) {
    if (positional && glm::distance2(_listenerPosition, position) > kMaxPositionalSoundDistance2) {
        return nullptr;
    }
    auto sound = make_shared<SoundInstance>(stream, loop, getGain(type, gain), positional, move(position));
    enqueue(sound);
    return sound->handle();
}

void AudioPlayer::setListenerPosition(glm::vec3 position) {
    if (_listenerPosition == position) {
        return;
    }
    _listenerPosition = move(position);
    _listenerPositionDirty = true;
}

} // namespace audio

} // namespace reone
