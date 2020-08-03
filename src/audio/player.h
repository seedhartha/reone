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

#pragma once

#include <atomic>
#include <list>
#include <mutex>
#include <thread>

#include "AL/alc.h"

#include "soundinstance.h"

namespace reone {

namespace audio {

class AudioPlayer {
public:
    static AudioPlayer &instance();

    void init(const AudioOptions &opts);
    void deinit();
    void play(const std::shared_ptr<AudioStream> &stream, bool loop = false);
    void reset();

private:
    AudioOptions _opts;
    ALCdevice *_device { nullptr };
    ALCcontext *_context { nullptr };
    std::thread _thread;
    std::atomic_bool _run { true };
    std::list<SoundInstance> _sounds;
    std::recursive_mutex _soundsMutex;

    AudioPlayer() = default;
    AudioPlayer(const AudioPlayer &) = delete;
    ~AudioPlayer();

    AudioPlayer &operator=(const AudioPlayer &) = delete;

    static void threadStart(AudioPlayer *);
};

#define TheAudioPlayer audio::AudioPlayer::instance()

} // namespace audio

} // namespace reone
