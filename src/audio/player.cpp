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

#include "player.h"

#include <algorithm>
#include <functional>
#include <stdexcept>

#include "AL/al.h"

#include "../core/log.h"

namespace reone {

namespace audio {

AudioPlayer &AudioPlayer::instance() {
    static AudioPlayer instance;
    return instance;
}

void AudioPlayer::init(const AudioOptions &opts) {
    _opts = opts;
    if (_opts.volume == 0) {
        info("Volume set to 0, disabling audio");
        return;
    }

    _device = alcOpenDevice(nullptr);
    if (!_device) {
        throw std::runtime_error("Failed to open audio device");
    }

    _context = alcCreateContext(_device, nullptr);
    if (!_context) {
        throw std::runtime_error("Failed to create audio context");
    }
    alcMakeContextCurrent(_context);
    alListenerf(AL_GAIN, _opts.volume / 100.0f);

    _thread = std::thread(std::bind(threadStart, this));
}

void AudioPlayer::threadStart(AudioPlayer *player) {
    while (player->_run) {
        std::lock_guard<std::recursive_mutex> lock(player->_soundsMutex);
        std::remove_if(
            player->_sounds.begin(),
            player->_sounds.end(),
            [](const SoundInstance &sound) { return sound.stopped(); });

        for (auto &sound : player->_sounds) {
            sound.update();
        }
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

void AudioPlayer::play(const std::shared_ptr<AudioStream> &stream, bool loop) {
    if (!stream) {
        throw std::invalid_argument("Audio stream is empty");
    }
    SoundInstance sound(stream, loop);
    std::lock_guard<std::recursive_mutex> lock(_soundsMutex);
    _sounds.push_back(std::move(sound));
}

void AudioPlayer::reset() {
    std::lock_guard<std::recursive_mutex> lock(_soundsMutex);
    _sounds.clear();
}

} // namespace audio

} // namespace reone
