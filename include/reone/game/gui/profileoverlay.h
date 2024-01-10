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
#include "reone/graphics/uniformbuffer.h"
#include "reone/input/event.h"
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

    ~ProfileOverlay() {
        deinit();
    }

    void init();
    void deinit();

    bool handle(const input::Event &event);
    void update(float dt);
    void render();

    bool timeInput(std::function<bool()> block);
    void timeUpdate(std::function<void()> block);
    void timeRender(std::function<void()> block);

    void onFrameEnded();

private:
    struct FrameTimes {
        float input;
        float update;
        float render;
        float total;

        FrameTimes(float input, float update, float render) :
            input(input),
            update(update),
            render(render),
            total(input + update + render) {
        }
    };

    ServicesView &_services;
    OptionsView &_options;

    bool _inited {false};
    bool _enabled {false};

    std::deque<FrameTimes> _frameTimes;
    float _inputTime {0.0f};
    float _updateTime {0.0f};
    float _renderTime {0.0f};

    Timer _percentilesTimer;
    float _p99FrameTime {0.0f};
    float _p95FrameTime {0.0f};

    std::shared_ptr<graphics::Font> _font;
    std::shared_ptr<graphics::UniformBuffer> _frameTimesUBO;

    void renderBackground();
    void renderFrameTimes();
    void renderText();
};

} // namespace game

} // namespace reone
