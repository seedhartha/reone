/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "game.h"

#include "SDL2/SDL.h"

#include "../audio/player.h"
#include "../core/jobs.h"
#include "../core/log.h"
#include "../core/streamutil.h"
#include "../resources/manager.h"
#include "../script/routines.h"

using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

Game::Game(
    GameVersion version,
    const fs::path &path,
    const std::string &startModule,
    const Options &opts
) :
    _version(version),
    _path(path),
    _startModule(startModule),
    _opts(opts),
    _renderWindow(opts.graphics, this) {
}

int Game::run() {
    _renderWindow.init();
    _renderWindow.setRenderWorldFunc(std::bind(&Game::renderWorld, this));
    _renderWindow.setRenderGUIFunc(std::bind(&Game::renderGUI, this));

    ResMan.init(_version, _path);
    TheAudioPlayer.init(_opts.audio);
    RoutineMan.init(_version);

    configure();

    _renderWindow.show();
    runMainLoop();

    TheJobExecutor.deinit();
    ResMan.deinit();
    TheAudioPlayer.deinit();
    _renderWindow.deinit();

    return 0;
}

void Game::configure() {
    const std::vector<std::string> &moduleNames = ResourceManager::instance().moduleNames();
    if (_startModule.empty() || std::find(moduleNames.begin(), moduleNames.end(), _startModule) == moduleNames.end()) {
        std::unique_ptr<MainMenu> mainMenu(new MainMenu(_opts.graphics));
        mainMenu->load(_version);
        mainMenu->initGL();
        mainMenu->setOnNewGame(std::bind(&Game::startModuleSelection, this));
        mainMenu->setOnExit([this]() { _quit = true; });

        _mainMenu = std::move(mainMenu);
        _screen = Screen::MainMenu;

    } else {
        loadModule(_startModule);
    }
}

void Game::startModuleSelection() {
    std::unique_ptr<ModulesGui> modules(new ModulesGui(_opts.graphics));
    modules->load();
    modules->initGL();
    modules->setOnModuleSelected([this](const std::string &name) { loadModule(name); });

    _modulesGui = std::move(modules);
    _screen = Screen::ModuleSelection;
}

void Game::loadModule(const std::string &name, std::string entry) {
    info("Loading module " + name);

    ResourceManager &resources = ResourceManager::instance();
    resources.loadModule(name);

    std::shared_ptr<GffStruct> ifo(resources.findGFF("module", ResourceType::ModuleInfo));

    _module = makeModule(name);
    configureModule();

    _module->load(*ifo, entry);
    _module->area().loadState(_state);
    _module->initGL();

    AudioPlayer &audio = AudioPlayer::instance();
    audio.reset();

    std::string musicName(_module->area().music());
    if (!musicName.empty()) {
        std::shared_ptr<AudioStream> music(resources.findAudio(musicName));
        audio.play(music, true);
    }

    if (!_hud) {
        std::unique_ptr<HUD> hud(new HUD(_opts.graphics));
        hud->load(_version);
        hud->initGL();

        _hud = std::move(hud);
    }
    if (!_debug) {
        std::unique_ptr<DebugGui> debug(new DebugGui(_opts.graphics));
        debug->load();
        debug->initGL();

        _debug = std::move(debug);
    }

    _ticks = SDL_GetTicks();
    _screen = Screen::InGame;
}

const std::shared_ptr<Module> Game::makeModule(const std::string &name) {
    return std::make_shared<Module>(name, _version, _opts.graphics);
}

void Game::configureModule() {
    _module->setOnCameraChanged([this](CameraType type) {
        _renderWindow.setRelativeMouseMode(type == CameraType::FirstPerson);
    });
    _module->setOnModuleTransition([this](const std::string &name, const std::string &entry) {
        _nextModule = name;
        _nextEntry = entry;
    });
}

void Game::runMainLoop() {
    _ticks = SDL_GetTicks();

    while (!_quit) {
        _renderWindow.processEvents(_quit);
        update();

        std::shared_ptr<Camera> camera(_module ? _module->getCamera() : nullptr);
        _renderWindow.render(camera);
    }
}

void Game::update() {
    if (!_nextModule.empty()) {
        loadNextModule();
    }
    float dt = getDeltaTime();

    if (_screen == Screen::InGame && _module) {
        gui::GuiContext guiCtx;
        _module->update(dt, guiCtx);

        if (_module->cameraType() == CameraType::ThirdPerson) {
            _hud->update(guiCtx.hud);
        }

        _debug->update(guiCtx.debug);
    }
}

void Game::loadNextModule() {
    info("Awaiting async jobs completion");
    JobExecutor &jobs = JobExecutor::instance();
    jobs.cancel();
    jobs.await();

    if (_module) {
        _module->area().saveTo(_state);
    }
    loadModule(_nextModule, _nextEntry);

    _nextModule.clear();
    _nextEntry.clear();
}

float Game::getDeltaTime() {
    uint32_t ticks = SDL_GetTicks();
    float dt = (ticks - _ticks) / 1000.0f;
    _ticks = ticks;

    return dt;
}

std::shared_ptr<GUI> Game::currentGUI() const {
    switch (_screen) {
        case Screen::MainMenu:
            return _mainMenu;
        case Screen::ModuleSelection:
            return _modulesGui;
        case Screen::InGame:
            return _hud;
        default:
            return nullptr;
    }
}

bool Game::handle(const SDL_Event &event) {
    switch (_screen) {
        case Screen::MainMenu:
            if (_mainMenu->handle(event)) return true;
            break;
        case Screen::ModuleSelection:
            if (_modulesGui->handle(event)) return true;
            break;
        case Screen::InGame:
            if (_module->cameraType() == CameraType::ThirdPerson && _hud->handle(event)) return true;
            if (_module->handle(event)) return true;
            break;
        default:
            break;
    }

    return false;
}

void Game::renderWorld() {
    if (_screen == Screen::InGame && _module) {
        _module->render();
    }
}

void Game::renderGUI() {
    switch (_screen) {
        case Screen::MainMenu:
            _mainMenu->render();
            break;
        case Screen::InGame:
            _debug->render();
            if (_module->cameraType() == CameraType::ThirdPerson) _hud->render();
            break;
        case Screen::ModuleSelection:
            _modulesGui->render();
            break;
        default:
            break;
    }
}

} // namespace game

} // namespace reone
