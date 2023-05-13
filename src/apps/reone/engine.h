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

#include "module/audio.h"
#include "module/game.h"
#include "module/graphics.h"
#include "module/resource.h"
#include "module/scene.h"
#include "module/script.h"
#include "module/system.h"

#include "options.h"

namespace reone {

class Engine : boost::noncopyable {
public:
    Engine(int argc, char **argv) :
        _argc(argc),
        _argv(argv) {
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
    int _argc;
    char **_argv;

    std::unique_ptr<SystemModule> _systemModule;
    std::unique_ptr<ResourceModule> _resourceModule;
    std::unique_ptr<GraphicsModule> _graphicsModule;
    std::unique_ptr<AudioModule> _audioModule;
    std::unique_ptr<SceneModule> _sceneModule;
    std::unique_ptr<ScriptModule> _scriptModule;
    std::unique_ptr<GameModule> _gameModule;

    std::unique_ptr<Options> _options;
    std::unique_ptr<game::OptionsView> _optionsView;

    std::unique_ptr<game::ServicesView> _services;
    std::unique_ptr<game::Game> _game;

    void loadOptions();
    void initServices(game::GameID gameId);
};

} // namespace reone
