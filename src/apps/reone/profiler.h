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

namespace reone {

namespace graphics {

struct GraphicsOptions;
struct GraphicsServices;

} // namespace graphics

namespace resource {

struct ResourceServices;

}

struct SystemServices;

class Profiler : boost::noncopyable {
public:
    Profiler(graphics::GraphicsOptions &graphicsOpt,
             graphics::GraphicsServices &graphicsSvc,
             resource::ResourceServices &resourceSvc,
             SystemServices &systemSvc) :
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc),
        _systemSvc(systemSvc) {
    }

    ~Profiler() {
        deinit();
    }

    void init();
    void deinit();

    bool handle(const input::Event &event);
    void update(float dt);
    void render();

    void timeInput(std::function<void()> block);
    void timeUpdate(std::function<void()> block);
    void timeRender(std::function<void()> block);

private:
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;
    SystemServices &_systemSvc;

    bool _inited {false};
    std::atomic_bool _enabled {false};

    std::deque<float> _inputTimes;
    std::deque<float> _updateTimes;
    std::deque<float> _renderTimes;

    Timer _percentilesTimer;
    float _p99FrameTime {0.0f};
    float _p95FrameTime {0.0f};

    std::shared_ptr<graphics::Font> _font;
    std::mutex _mutex;

    void renderBackground();
    void renderFrameTimes();
    void renderText();
};

} // namespace reone
