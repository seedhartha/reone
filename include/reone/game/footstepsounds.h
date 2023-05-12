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

#include "reone/audio/stream.h"
#include "reone/system/memorycache.h"

namespace reone {

namespace resource {

class TwoDas;

}

namespace audio {

class AudioFiles;

}

namespace game {

struct FootstepTypeSounds {
    std::vector<std::shared_ptr<audio::AudioStream>> dirt;
    std::vector<std::shared_ptr<audio::AudioStream>> grass;
    std::vector<std::shared_ptr<audio::AudioStream>> stone;
    std::vector<std::shared_ptr<audio::AudioStream>> wood;
    std::vector<std::shared_ptr<audio::AudioStream>> water;
    std::vector<std::shared_ptr<audio::AudioStream>> carpet;
    std::vector<std::shared_ptr<audio::AudioStream>> metal;
    std::vector<std::shared_ptr<audio::AudioStream>> leaves;
};

class IFootstepSounds {
public:
    virtual ~IFootstepSounds() = default;
};

class FootstepSounds : public IFootstepSounds, public MemoryCache<uint32_t, FootstepTypeSounds> {
public:
    FootstepSounds(
        audio::AudioFiles &audioFiles,
        resource::TwoDas &twoDas) :
        MemoryCache(std::bind(&FootstepSounds::doGet, this, std::placeholders::_1)),
        _audioFiles(audioFiles),
        _twoDas(twoDas) {
    }

    std::shared_ptr<FootstepTypeSounds> doGet(uint32_t type);

private:
    audio::AudioFiles &_audioFiles;
    resource::TwoDas &_twoDas;
};

} // namespace game

} // namespace reone
