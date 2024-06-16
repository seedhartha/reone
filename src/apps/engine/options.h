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

#include "reone/audio/options.h"
#include "reone/game/options.h"
#include "reone/graphics/options.h"
#include "reone/system/types.h"

namespace reone {

struct Options {
    struct Logging {
        LogSeverity severity {LogSeverity::Info};
        std::set<LogChannel> channels {LogChannel::Global};
    };

    game::GameOptions game;
    graphics::GraphicsOptions graphics;
    audio::AudioOptions audio;

    Logging logging;

    std::unique_ptr<game::OptionsView> toView() {
        return std::make_unique<game::OptionsView>(game, graphics, audio);
    }
};

} // namespace reone
