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
#include "../resources/resources.h"

#include "script/routines.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

Game::Game(GameVersion version, const fs::path &path, const Options &opts) :
    _version(version),
    _path(path),
    _opts(opts),
    _renderWindow(opts.graphics, this) {
}

int Game::run() {
    _renderWindow.init();
    _renderWindow.setRenderWorldFunc(bind(&Game::renderWorld, this));
    _renderWindow.setRenderGUIFunc(bind(&Game::renderGUI, this));

    ResMan.init(_version, _path);
    TheAudioPlayer.init(_opts.audio);
    initScriptRoutines(_version);

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
    loadMainMenu();
    _screen = Screen::MainMenu;
}

void Game::loadMainMenu() {
    unique_ptr<MainMenu> mainMenu(new MainMenu(_opts));
    mainMenu->load(_version);
    mainMenu->initGL();
    mainMenu->setOnNewGame([this]() {
        _mainMenu->resetFocus();
        if (!_classSelection) loadClassSelectionGui();
        _screen = Screen::ClassSelection;
    });
    mainMenu->setOnExit([this]() { _quit = true; });
    mainMenu->setOnModuleSelected([this](const string &name) { loadModule(name, PartyConfiguration()); });
    _mainMenu = move(mainMenu);
}

void Game::loadClassSelectionGui() {
    unique_ptr<ClassSelectionGui> gui(new ClassSelectionGui(_opts.graphics));
    gui->load(_version);
    gui->initGL();
    gui->setOnClassSelected([this](const CharacterConfiguration &character) {
        _classSelection->resetFocus();
        if (!_portraits) loadPortraitsGui();
        _portraits->loadPortraits(character);
        _screen = Screen::Portraits;
    });
    gui->setOnCancel([this]() {
        _classSelection->resetFocus();
        _screen = Screen::MainMenu;
    });
    _classSelection = move(gui);
}

void Game::loadPortraitsGui() {
    unique_ptr<PortraitsGui> gui(new PortraitsGui(_opts.graphics));
    gui->load(_version);
    gui->initGL();
    gui->setOnPortraitSelected([this](const CharacterConfiguration &character) {
        _portraits->resetFocus();
        string moduleName(_version == GameVersion::KotOR ? "end_m01aa" : "001ebo");

        PartyConfiguration party;
        party.leader = character;

        loadModule(moduleName, party);
    });
    gui->setOnCancel([this]() {
        _screen = Screen::ClassSelection;
    });
    _portraits = move(gui);
}

void Game::loadModule(const string &name, const PartyConfiguration &party, string entry) {
    info("Loading module " + name);
    ResMan.loadModule(name);

    shared_ptr<GffStruct> ifo(ResMan.findGFF("module", ResourceType::ModuleInfo));

    _module = makeModule(name);
    configureModule();

    _module->load(*ifo);
    _module->loadParty(party, entry);
    _module->area().loadState(_state);
    _module->initGL();

    TheAudioPlayer.reset();
    string musicName(_module->area().music());
    if (!musicName.empty()) {
        shared_ptr<AudioStream> music(ResMan.findAudio(musicName));
        TheAudioPlayer.play(music, AudioType::Music);
    }

    if (!_hud) loadHUD();
    if (!_debug) loadDebugGui();
    if (!_dialog) loadDialogGui();

    _ticks = SDL_GetTicks();
    _screen = Screen::InGame;
}

void Game::loadHUD() {
    unique_ptr<HUD> hud(new HUD(_opts.graphics));
    hud->load(_version);
    hud->initGL();
    _hud = move(hud);
}

void Game::loadDebugGui() {
    unique_ptr<DebugGui> debug(new DebugGui(_opts.graphics));
    debug->load();
    debug->initGL();
    _debug = move(debug);
}

void Game::loadDialogGui() {
    unique_ptr<DialogGui> dialog(new DialogGui(_opts.graphics));
    dialog->load(_version);
    dialog->initGL();
    dialog->setOnSpeakerChanged([this](const string &from, const string &to) {
        Object *player = _module->area().player().get();
        Creature *prevSpeaker = !from.empty() ? static_cast<Creature *>(_module->area().find(from).get()) : nullptr;
        Creature *speaker = !to.empty() ? static_cast<Creature *>(_module->area().find(to).get()) : nullptr;

        if (speaker == player) return;

        if (prevSpeaker) {
            prevSpeaker->playDefaultAnimation();
        }
        if (player && speaker) {
            player->face(*speaker);
            speaker->face(*player);
            speaker->playTalkAnimation();
            _module->update3rdPersonCameraHeading();
        }
    });
    dialog->setOnDialogFinished([this]() {
        _screen = Screen::InGame;
    });
    _dialog = move(dialog);
}

const shared_ptr<Module> Game::makeModule(const string &name) {
    return make_shared<Module>(name, _version, _opts.graphics);
}

void Game::configureModule() {
    _module->setOnCameraChanged([this](CameraType type) {
        _renderWindow.setRelativeMouseMode(type == CameraType::FirstPerson);
    });
    _module->setOnModuleTransition([this](const string &name, const string &entry) {
        _nextModule = name;
        _nextEntry = entry;
    });
    _module->setStartDialog([this](const Object &object, const string &resRef) {
        _screen = Screen::Dialog;
        _dialog->startDialog(object, resRef);
    });
}

void Game::runMainLoop() {
    _ticks = SDL_GetTicks();

    while (!_quit) {
        _renderWindow.processEvents(_quit);
        update();

        shared_ptr<Camera> camera(_module ? _module->getCamera() : nullptr);
        _renderWindow.render(camera);
    }
}

void Game::update() {
    if (!_nextModule.empty()) {
        loadNextModule();
    }
    float dt = getDeltaTime();
    bool updModule = _screen == Screen::InGame || _screen == Screen::Dialog;

    if (updModule && _module) {
        GuiContext guiCtx;
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
        _module->saveTo(_state);
    }
    loadModule(_nextModule, _state.party, _nextEntry);

    _nextModule.clear();
    _nextEntry.clear();
}

float Game::getDeltaTime() {
    uint32_t ticks = SDL_GetTicks();
    float dt = (ticks - _ticks) / 1000.0f;
    _ticks = ticks;

    return dt;
}

shared_ptr<GUI> Game::currentGUI() const {
    switch (_screen) {
        case Screen::MainMenu:
            return _mainMenu;
        case Screen::ClassSelection:
            return _classSelection;
        case Screen::Portraits:
            return _portraits;
        case Screen::InGame:
            return _hud;
        case Screen::Dialog:
            return _dialog;
        default:
            return nullptr;
    }
}

bool Game::handle(const SDL_Event &event) {
    switch (_screen) {
        case Screen::InGame:
            if (_module->cameraType() == CameraType::ThirdPerson && _hud->handle(event)) return true;
            if (_module->handle(event)) return true;
            break;
        default:
            if (currentGUI()->handle(event)) return true;
            break;
    }

    return false;
}

void Game::renderWorld() {
    switch (_screen) {
        case Screen::InGame:
        case Screen::Dialog:
            if (_module) _module->render();
            break;
        default:
            break;
    }
}

void Game::renderGUI() {
    switch (_screen) {
        case Screen::InGame:
            _debug->render();
            if (_module->cameraType() == CameraType::ThirdPerson) _hud->render();
            break;
        default:
            currentGUI()->render();
            break;
    }
}

} // namespace game

} // namespace reone
