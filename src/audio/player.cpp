/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <algorithm>
#include <functional>
#include <stdexcept>

#include "AL/al.h"

#include "../common/log.h"

#include "soundhandle.h"

using namespace std;

namespace reone {

namespace audio {

AudioPlayer &AudioPlayer::instance() {
    static AudioPlayer instance;
    return instance;
}

void AudioPlayer::init(const AudioOptions &opts) {
    _opts = opts;

    if (_opts.musicVolume == 0 && _opts.soundVolume == 0 && _opts.movieVolume == 0) {
        info("AudioPlayer: audio is disabled");
        return;
    }
    _thread = thread(bind(&AudioPlayer::threadStart, this));
}

void AudioPlayer::threadStart() {
    initAL();

    vector<shared_ptr<SoundInstance>> sounds;
    while (_run) {
        {
            lock_guard<recursive_mutex> lock(_soundsMutex);
            auto maybeSounds = remove_if(
                _sounds.begin(), _sounds.end(),
                [](auto &sound) { return sound->handle()->isStopped(); });

            _sounds.erase(maybeSounds, _sounds.end());
            sounds = _sounds;
        }
        if (_listenerPositionDirty) {
            Vector3 position(_listenerPosition.load());
            alListener3f(AL_POSITION, position.x, position.y, position.z);
            _listenerPositionDirty = false;
        }
        for (auto &sound : sounds) {
            if (sound->handle()->isNotInited()) {
                sound->init();
            }
            sound->update();
        }
        this_thread::yield();
    }

    deinitAL();
}

void AudioPlayer::initAL() {
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

void AudioPlayer::deinitAL() {
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

AudioPlayer::~AudioPlayer() {
    deinit();
}

void AudioPlayer::deinit() {
    _run = false;

    if (_thread.joinable()) {
        _thread.join();
    }
}

shared_ptr<SoundHandle> AudioPlayer::play(const string &resRef, AudioType type, bool loop, float gain, bool positional, Vector3 position) {
    shared_ptr<SoundInstance> sound(new SoundInstance(resRef, loop, getGain(type, gain), positional, move(position)));
    enqueue(sound);
    return sound->handle();
}

float AudioPlayer::getGain(AudioType type, float gain) const {
    int volume;
    switch (type) {
        case AudioType::Music:
            volume = _opts.musicVolume;
            break;
        case AudioType::Sound:
            volume = _opts.soundVolume;
            break;
        case AudioType::Movie:
            volume = _opts.movieVolume;
            break;
        default:
            return 85;
    }

    return gain * (volume / 100.0f);
}

void AudioPlayer::enqueue(const shared_ptr<SoundInstance> &sound) {
    lock_guard<recursive_mutex> lock(_soundsMutex);
    _sounds.push_back(sound);
}

shared_ptr<SoundHandle> AudioPlayer::play(const shared_ptr<AudioStream> &stream, AudioType type, bool loop, float gain, bool positional, Vector3 position) {
    shared_ptr<SoundInstance> sound(new SoundInstance(stream, loop, getGain(type, gain), positional, move(position)));
    enqueue(sound);
    return sound->handle();
}

void AudioPlayer::setListenerPosition(Vector3 position) {
    if (_listenerPosition.load() != position) {
        _listenerPosition = position;
        _listenerPositionDirty = true;
    }
}

} // namespace audio

} // namespace reone
