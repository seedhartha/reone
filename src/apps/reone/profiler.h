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

#include "reone/game/profiler.h"
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

class Profiler : public game::IProfiler, boost::noncopyable {
public:
    static constexpr int kMaxTimedThreads = 4;

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

    void reserveThread(std::string name) override;

    void measure(const std::string &threadName,
                 int timeIndex,
                 const std::function<void()> &block) override;

private:
    struct TimedThread {
        std::string name;
        std::array<std::deque<float>, 4> times;
        std::mutex mutex;
    };

    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;
    SystemServices &_systemSvc;

    bool _inited {false};
    std::atomic_bool _enabled {false};
    float _fpsTarget {60.0f};

    std::array<TimedThread, kMaxTimedThreads> _timedThreads;
    int _numTimedThreads {0};
    std::map<std::string, std::reference_wrapper<TimedThread>> _nameToTimedThread;

    std::shared_ptr<graphics::Font> _font;

    void renderBackground();
    void renderFrameTimes(const TimedThread &thread, int xOffset);
};

} // namespace reone
