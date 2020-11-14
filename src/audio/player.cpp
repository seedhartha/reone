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
        info("Audio disabled");
        return;
    }

    _device = alcOpenDevice(nullptr);
    if (!_device) {
        throw runtime_error("Failed to open audio device");
    }

    _context = alcCreateContext(_device, nullptr);
    if (!_context) {
        throw runtime_error("Failed to create audio context");
    }
    alcMakeContextCurrent(_context);

    _thread = thread(bind(&AudioPlayer::threadStart, this));
}

void AudioPlayer::threadStart() {
    vector<shared_ptr<SoundInstance>> sounds;
    while (_run) {
        {
            lock_guard<recursive_mutex> lock(_soundsMutex);
            auto it = remove_if(
                _sounds.begin(),
                _sounds.end(),
                [](const shared_ptr<SoundInstance> &sound) { return sound->isStopped(); });

            _sounds.erase(it, _sounds.end());
            sounds = _sounds;
        }
        for (auto &sound : sounds) {
            sound->update();
        }
        this_thread::yield();
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
    _sounds.clear();

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

void AudioPlayer::reset() {
    lock_guard<recursive_mutex> lock(_soundsMutex);
    _sounds.clear();
}

shared_ptr<SoundInstance> AudioPlayer::play(const shared_ptr<AudioStream> &stream, AudioType type, bool loop, float gain) {
    if (!stream) {
        throw invalid_argument("Audio stream is empty");
    }
    float finalGain = gain * getVolume(type) / 100.0f;
    shared_ptr<SoundInstance> sound(new SoundInstance(stream, loop, finalGain));

    lock_guard<recursive_mutex> lock(_soundsMutex);
    _sounds.push_back(sound);

    return move(sound);
}

int AudioPlayer::getVolume(AudioType type) const {
    switch (type) {
        case AudioType::Music:
            return _opts.musicVolume;
        case AudioType::Sound:
            return _opts.soundVolume;
        case AudioType::Movie:
            return _opts.movieVolume;
        default:
            return 85;
    }
}

} // namespace audio

} // namespace reone
