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

#pragma once

#include "context.h"
#include "files.h"
#include "player.h"

namespace reone {

namespace audio {

struct AudioServices {
    IAudioContext &context;
    IAudioFiles &files;
    IAudioPlayer &player;

    AudioServices(
        IAudioContext &context,
        IAudioFiles &files,
        IAudioPlayer &player) :
        context(context),
        files(files),
        player(player) {
    }

    AudioContext &defaultContext() {
        auto casted = dynamic_cast<AudioContext *>(&context);
        if (!casted) {
            throw std::logic_error("Illegal AudioContext implementation");
        }
        return *casted;
    }

    AudioFiles &defaultFiles() {
        auto casted = dynamic_cast<AudioFiles *>(&files);
        if (!casted) {
            throw std::logic_error("Illegal AudioFiles implementation");
        }
        return *casted;
    }

    AudioPlayer &defaultPlayer() {
        auto casted = dynamic_cast<AudioPlayer *>(&player);
        if (!casted) {
            throw std::logic_error("Illegal AudioPlayer implementation");
        }
        return *casted;
    }
};

} // namespace audio

} // namespace reone
