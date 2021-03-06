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
#include "../../graphics/font.h"
#include "../../graphics/services.h"

namespace reone {

namespace game {

class ProfileOverlay {
public:
    ProfileOverlay(graphics::GraphicsServices &graphics);

    void init();
    bool handle(const SDL_Event &event);
    void update(float dt);
    void draw();

private:
    struct FPS {
        int average { 0 };
        int onePerLow { 0 }; /**< 1% Low */
    };

    graphics::GraphicsServices &_graphics;

    uint64_t _frequency { 0 };
    uint64_t _counter { 0 };
    std::shared_ptr<graphics::Font> _font;

    bool _enabled { false };

    Timer _refreshTimer;
    std::vector<float> _frametimes;
    FPS _fps;

    void calculateFPS();

    void drawBackground();
    void drawText();
};

} // namespace game

} // namespace reone
