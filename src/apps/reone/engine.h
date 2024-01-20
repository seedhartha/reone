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

#include "reone/audio/di/module.h"
#include "reone/game/di/module.h"
#include "reone/game/game.h"
#include "reone/game/neo/game.h"
#include "reone/graphics/di/module.h"
#include "reone/graphics/window.h"
#include "reone/gui/di/module.h"
#include "reone/input/event.h"
#include "reone/movie/di/module.h"
#include "reone/resource/di/module.h"
#include "reone/scene/di/module.h"
#include "reone/script/di/module.h"
#include "reone/system/di/module.h"

#include "options.h"
#include "profiler.h"

namespace reone {

class Engine : boost::noncopyable {
public:
    Engine(Options &options) :
        _options(options) {
    }

    ~Engine() {
        deinit();
    }

    void init();
    void deinit();

    /**
     * @return exit code
     */
    int run();

private:
    struct FrameStates {
        static constexpr int rendered = 0;
        static constexpr int updating = 1;
        static constexpr int updated = 2;
        static constexpr int rendering = 3;
    };

    Options &_options;

    std::unique_ptr<game::OptionsView> _optionsView;
    std::unique_ptr<graphics::Window> _window;

    std::unique_ptr<Clock> _clock;
    std::unique_ptr<SystemModule> _systemModule;
    std::unique_ptr<resource::ResourceModule> _resourceModule;
    std::unique_ptr<graphics::GraphicsModule> _graphicsModule;
    std::unique_ptr<audio::AudioModule> _audioModule;
    std::unique_ptr<movie::MovieModule> _movieModule;
    std::unique_ptr<scene::SceneModule> _sceneModule;
    std::unique_ptr<gui::GUIModule> _guiModule;
    std::unique_ptr<script::ScriptModule> _scriptModule;
    std::unique_ptr<game::GameModule> _gameModule;

    std::unique_ptr<game::ServicesView> _services;
    std::unique_ptr<game::Game> _game;
    std::unique_ptr<game::neo::Game> _neoGame;
    std::unique_ptr<Profiler> _profiler;

    std::atomic_bool _quit {false};
    std::atomic_bool _focus {false};
    std::atomic_int _frameState {FrameStates::rendered};

    std::queue<input::Event> _events;
    std::mutex _eventsMutex;

    uint32_t _ticks {0};

    bool _showCursor {true};
    bool _relativeMouseMode {false};

    void gameThreadFunc();

    void processEvents(bool &quit);

    void showCursor(bool show);
    void setRelativeMouseMode(bool relative);

    std::optional<input::Event> eventFromSDLEvent(const SDL_Event &sdlEvent) const;
};

} // namespace reone
