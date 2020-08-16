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
#include "util.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

static const int kAppearanceBastila = 4;
static const int kAppearanceCarth = 6;
static const int kAppearanceDarthRevan = 22;
static const int kAppearanceAtton = 452;
static const int kAppearanceKreia = 455;

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
    RoutineMan.init(_version, this);

    Cursor cursor;
    cursor.pressed = ResMan.findTexture("gui_mp_defaultd", TextureType::GUI);
    cursor.pressed->initGL();
    cursor.unpressed = ResMan.findTexture("gui_mp_defaultu", TextureType::GUI);
    cursor.unpressed->initGL();

    _renderWindow.setCursor(cursor);

    configure();
    _renderWindow.show();
    runMainLoop();

    TheJobExecutor.deinit();
    RoutineMan.deinit();
    TheAudioPlayer.deinit();
    ResMan.deinit();
    _renderWindow.deinit();

    return 0;
}

void Game::configure() {
    loadMainMenu();
    _screen = Screen::MainMenu;

    if (_music) _music->stop();
    switch (_version) {
        case GameVersion::TheSithLords:
            _music = playMusic("mus_sion");
            break;
        default:
            _music = playMusic("mus_theme_cult");
            break;
    }
}

void Game::loadMainMenu() {
    unique_ptr<MainMenu> mainMenu(new MainMenu(_opts));
    mainMenu->load(_version);
    mainMenu->initGL();
    mainMenu->setOnNewGame([this]() {
        _mainMenu->resetFocus();
        if (!_classesGui) loadClassSelectionGui();

        if (_music) _music->stop();
        switch (_version) {
            case GameVersion::TheSithLords:
                _music = playMusic("mus_main");
                break;
            default:
                _music = playMusic("mus_theme_rep");
                break;
        }

        _screen = Screen::ClassSelection;
    });
    mainMenu->setOnExit([this]() { _quit = true; });
    mainMenu->setOnModuleSelected([this](const string &name) {
        PartyConfiguration party;
        party.memberCount = 2;
        party.leader.equipment.push_back("g_a_clothes01");
        party.member1.equipment.push_back("g_a_clothes01");

        switch (_version) {
            case GameVersion::TheSithLords:
                party.leader.appearance = kAppearanceAtton;
                party.member1.appearance = kAppearanceKreia;
                break;
            default:
                party.leader.appearance = kAppearanceCarth;
                party.member1.appearance = kAppearanceBastila;
                break;
        }

        loadModule(name, party);
    });
    _mainMenu = move(mainMenu);
}

void Game::loadClassSelectionGui() {
    unique_ptr<ClassSelectionGui> gui(new ClassSelectionGui(_opts.graphics));
    gui->load(_version);
    gui->initGL();
    gui->setOnClassSelected([this](const CreatureConfiguration &character) {
        _classesGui->resetFocus();
        if (!_portraitsGui) loadPortraitsGui();
        _portraitsGui->loadPortraits(character);
        _screen = Screen::PortraitSelection;
    });
    gui->setOnCancel([this]() {
        _classesGui->resetFocus();
        _screen = Screen::MainMenu;
    });
    _classesGui = move(gui);
}

void Game::loadPortraitsGui() {
    unique_ptr<PortraitSelectionGui> gui(new PortraitSelectionGui(_opts.graphics));
    gui->load(_version);
    gui->initGL();
    gui->setOnPortraitSelected([this](const CreatureConfiguration &character) {
        _portraitsGui->resetFocus();
        string moduleName(_version == GameVersion::KotOR ? "end_m01aa" : "001ebo");

        PartyConfiguration party;
        party.memberCount = 1;
        party.leader = character;

        loadModule(moduleName, party);
    });
    gui->setOnCancel([this]() {
        _screen = Screen::ClassSelection;
    });
    _portraitsGui = move(gui);
}

void Game::loadModule(const string &name, const PartyConfiguration &party, string entry) {
    info("Game: load module: " + name);
    ResMan.loadModule(name);

    shared_ptr<GffStruct> ifo(ResMan.findGFF("module", ResourceType::ModuleInfo));

    _module = makeModule(name);
    configureModule();

    _module->load(*ifo);
    _module->loadParty(party, entry);
    _module->area().loadState(_state);
    _module->initGL();

    if (_music) _music->stop();
    string musicName(_module->area().music());
    if (!musicName.empty()) {
        _music = playMusic(musicName);
    }

    if (!_hud) loadHUD();
    if (!_debugGui) loadDebugGui();
    if (!_dialogGui) loadDialogGui();

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
    _debugGui = move(debug);
}

void Game::loadDialogGui() {
    unique_ptr<DialogGui> dialog(new DialogGui(_opts.graphics));
    dialog->load(_version);
    dialog->initGL();
    dialog->setPickReplyEnabled(_pickDialogReplyEnabled);
    dialog->setGetObjectIdByTagFunc([this](const string &tag) {
        shared_ptr<Object> object(_module->area().find(tag, ObjectType::Creature));
        return object ? object->id() : 0;
    });
    dialog->setOnReplyPicked(bind(&Game::onDialogReplyPicked, this, _1));
    dialog->setOnSpeakerChanged(bind(&Game::onDialogSpeakerChanged, this, _1, _2));
    dialog->setOnDialogFinished(bind(&Game::onDialogFinished, this));
    _dialogGui = move(dialog);
}

void Game::onDialogReplyPicked(uint32_t index) {
}

void Game::onDialogSpeakerChanged(uint32_t from, uint32_t to) {
    shared_ptr<Object> player(_module->area().player());
    shared_ptr<Object> partyLeader(_module->area().partyLeader());
    shared_ptr<Object> prevSpeaker = from != 0 ? _module->area().find(from, ObjectType::Creature) : nullptr;
    shared_ptr<Object> speaker = to != 0 ? _module->area().find(to, ObjectType::Creature) : nullptr;
    if (speaker == partyLeader) return;

    debug(boost::format("Game: dialog speaker: \"%s\"") % (speaker ? speaker->tag() : ""));

    if (prevSpeaker) {
        static_cast<Creature &>(*prevSpeaker).setTalking(false);
    }
    if (speaker) {
        if (player) {
            player->face(*speaker);
            _module->update3rdPersonCameraHeading();
        }
        if (partyLeader) {
            partyLeader->face(*speaker);
            speaker->face(*partyLeader);
            static_cast<Creature &>(*speaker).setTalking(true);
        }
    }
}

void Game::onDialogFinished() {
    _screen = Screen::InGame;
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
    _module->setStartDialog([this](const Object &owner, const string &resRef) {
        startDialog(owner.id(), resRef);
    });
}

void Game::startDialog(uint32_t ownerId, const string &resRef) {
    _screen = Screen::Dialog;
    _dialogGui->startDialog(ownerId, resRef);
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

        _debugGui->update(guiCtx.debug);
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
            return _classesGui;
        case Screen::PortraitSelection:
            return _portraitsGui;
        case Screen::InGame:
            return _hud;
        case Screen::Dialog:
            return _dialogGui;
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
        default: {
            shared_ptr<GUI> gui(currentGUI());
            if (gui && gui->handle(event)) return true;
            break;
        }
    }

    return false;
}

void Game::delayCommand(uint32_t timestamp, const ExecutionContext &ctx) {
    _module->area().delayCommand(timestamp, ctx);
}

shared_ptr<Object> Game::getObjectById(uint32_t id) {
    return _module->area().find(id);
}

shared_ptr<Object> Game::getObjectByTag(const string &tag, int nth) {
    return _module->area().find(tag, nth);
}

shared_ptr<Object> Game::getWaypointByTag(const string &tag) {
    return _module->area().find(tag, ObjectType::Waypoint);
}

shared_ptr<Object> Game::getPlayer() {
    return _module->area().player();
}

int Game::eventUserDefined(int eventNumber) {
    return _module->area().eventUserDefined(eventNumber);
}

void Game::signalEvent(int eventId) {
    _module->area().signalEvent(eventId);
}

bool Game::getGlobalBoolean(const string &name) const {
    auto it = _state.globalBooleans.find(name);
    return it != _state.globalBooleans.end() ? it->second : false;
}

int Game::getGlobalNumber(const string &name) const {
    auto it = _state.globalNumbers.find(name);
    return it != _state.globalNumbers.end() ? it->second : 0;
}

bool Game::getLocalBoolean(uint32_t objectId, int index) const {
    assert(index >= 0);

    auto objectIt = _state.localBooleans.find(objectId);
    if (objectIt == _state.localBooleans.end()) return false;

    auto boolIt = objectIt->second.find(index);
    if (boolIt == objectIt->second.end()) return false;

    return boolIt->second;
}

int Game::getLocalNumber(uint32_t objectId, int index) const {
    auto objectIt = _state.localNumbers.find(objectId);
    if (objectIt == _state.localNumbers.end()) return 0;

    auto numberIt = objectIt->second.find(index);
    if (numberIt == objectIt->second.end()) return 0;

    return numberIt->second;
}

void Game::setGlobalBoolean(const string &name, bool value) {
    _state.globalBooleans[name] = value;
}

void Game::setGlobalNumber(const string &name, int value) {
    _state.globalNumbers[name] = value;
}

void Game::setLocalBoolean(uint32_t objectId, int index, bool value) {
    assert(index >= 0);
    _state.localBooleans[objectId][index] = value;
}

void Game::setLocalNumber(uint32_t objectId, int index, int value) {
    assert(index >= 0);
    _state.localBooleans[objectId][index] = value;
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
            _debugGui->render();
            if (_module->cameraType() == CameraType::ThirdPerson) _hud->render();
            break;
        default: {
            shared_ptr<GUI> gui(currentGUI());
            if (gui) gui->render();
            break;
        }
    }
}

} // namespace game

} // namespace reone
