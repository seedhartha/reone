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

#include "../game/footstepsounds.h"

namespace reone {

namespace resource {

class TwoDas;

}

namespace audio {

class AudioFiles;

}

namespace kotor {

class FootstepSounds : public game::IFootstepSounds {
public:
    FootstepSounds(
        audio::AudioFiles &audioFiles,
        resource::TwoDas &twoDas) :
        IFootstepSounds(std::bind(&FootstepSounds::doGet, this, std::placeholders::_1)),
        _audioFiles(audioFiles),
        _twoDas(twoDas) {
    }

    std::shared_ptr<game::FootstepTypeSounds> doGet(uint32_t type);

private:
    audio::AudioFiles &_audioFiles;
    resource::TwoDas &_twoDas;
};

} // namespace kotor

} // namespace reone
