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

#include "../../common/timer.h"
#include "../../game/gui/profileoverlay.h"
#include "../../game/options.h"
#include "../../graphics/font.h"

namespace reone {

namespace game {

struct Services;

}

namespace kotor {

class ProfileOverlay : public game::IProfileOverlay {
public:
    ProfileOverlay(game::Services &services, game::Options &options) :
        _services(services),
        _options(options) {
    }

    void init();

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void draw() override;

private:
    game::Services &_services;
    game::Options &_options;

    uint64_t _frequency {0};
    uint64_t _counter {0};
    std::shared_ptr<graphics::Font> _font;

    bool _enabled {false};

    Timer _refreshTimer;
    std::vector<float> _frametimes;
    int _fps {0};

    void calculateFPS();
};

} // namespace kotor

} // namespace reone
