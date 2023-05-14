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

#include "engine.h"

#include "reone/game/game.h"
#include "reone/system/logutil.h"

#include "gameprobe.h"
#include "optionsparser.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::movie;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

void Engine::init() {
    loadOptions();

    auto gameId = GameProbe(_options->game.path).probe();

    initServices(gameId);

    _game = make_unique<Game>(gameId, _options->game.path, *_optionsView, *_services);
    _game->init();
}

void Engine::loadOptions() {
    _options = OptionsParser(_argc, _argv).parse();
    _optionsView = _options->toView();

    setLogLevel(_options->logging.level);
    setLogChannels(_options->logging.channels);
    setLogToFile(_options->logging.logToFile);
}

void Engine::initServices(GameID gameId) {
    _systemModule = make_unique<SystemModule>();
    _resourceModule = make_unique<ResourceModule>(_options->game.path);
    _graphicsModule = make_unique<GraphicsModule>(_options->graphics, *_resourceModule);
    _audioModule = make_unique<AudioModule>(_options->audio, *_resourceModule);
    _movieModule = make_unique<MovieModule>(_options->game.path, *_graphicsModule, *_audioModule);
    _sceneModule = make_unique<SceneModule>(_options->graphics, *_audioModule, *_graphicsModule);
    _guiModule = make_unique<GUIModule>(*_resourceModule);
    _scriptModule = make_unique<ScriptModule>(*_resourceModule);

    _gameModule = make_unique<GameModule>(
        gameId,
        *_optionsView,
        *_resourceModule,
        *_graphicsModule,
        *_audioModule,
        *_sceneModule,
        *_scriptModule);

    _systemModule->init();
    _resourceModule->init();
    _graphicsModule->init();
    _audioModule->init();
    _movieModule->init();
    _sceneModule->init();
    _guiModule->init();
    _scriptModule->init();
    _gameModule->init();

    _services = make_unique<ServicesView>(
        _gameModule->services(),
        _movieModule->services(),
        _audioModule->services(),
        _graphicsModule->services(),
        _sceneModule->services(),
        _guiModule->services(),
        _scriptModule->services(),
        _resourceModule->services(),
        _systemModule->services());
}

void Engine::deinit() {
    _game.reset();
    _services.reset();

    _gameModule.reset();
    _scriptModule.reset();
    _guiModule.reset();
    _sceneModule.reset();
    _audioModule.reset();
    _graphicsModule.reset();
    _resourceModule.reset();
    _systemModule.reset();

    _optionsView.reset();
    _options.reset();
}

int Engine::run() {
    return _game->run();
}

} // namespace reone
