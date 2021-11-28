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

#pragma once

#include "options.h"
#include "types.h"

namespace reone {

namespace audio {

class AudioFiles;
class AudioStream;
class SoundHandle;
class SoundInstance;

class AudioPlayer : boost::noncopyable {
public:
    AudioPlayer(AudioOptions options, AudioFiles &audioFiles) :
        _options(std::move(options)),
        _audioFiles(audioFiles) {
    }

    ~AudioPlayer() { deinit(); }

    void init();
    void deinit();

    void update(float dt);

    std::shared_ptr<SoundHandle> play(const std::string &resRef, AudioType type, bool loop = false, float gain = 1.0f, bool positional = false, glm::vec3 position = glm::vec3(0.0f));
    std::shared_ptr<SoundHandle> play(const std::shared_ptr<AudioStream> &stream, AudioType type, bool loop = false, float gain = 1.0f, bool positional = false, glm::vec3 position = glm::vec3(0.0f));

    void setListenerPosition(glm::vec3 position);

private:
    AudioOptions _options;
    AudioFiles &_audioFiles;

    ALCdevice *_device {nullptr};
    ALCcontext *_context {nullptr};

    glm::vec3 _listenerPosition {0.0f};
    bool _listenerPositionDirty {false};

    std::vector<std::shared_ptr<SoundInstance>> _sounds;

    void enqueue(const std::shared_ptr<SoundInstance> &sound);

    float getGain(AudioType type, float gain) const;
};

} // namespace audio

} // namespace reone
