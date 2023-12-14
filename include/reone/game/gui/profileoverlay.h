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

#include "reone/graphics/font.h"
#include "reone/system/timer.h"

#include "../options.h"

namespace reone {

namespace game {

struct ServicesView;

class ProfileOverlay {
public:
    ProfileOverlay(ServicesView &services, OptionsView &options) :
        _services(services),
        _options(options) {
    }

    void init();

    bool handle(const SDL_Event &event);
    void update(float dt);
    void render();

private:
    ServicesView &_services;
    OptionsView &_options;

    bool _enabled {false};

    uint64_t _ticks {0};
    int _numFrames {0};
    int _fps {0};

    Timer _refreshTimer;
    std::shared_ptr<graphics::Font> _font;
};

} // namespace game

} // namespace reone
