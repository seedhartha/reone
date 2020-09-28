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

#include "GL/glew.h"

#include "SDL2/SDL.h"

#include "../audio/player.h"
#include "../core/jobs.h"
#include "../core/log.h"
#include "../core/pathutil.h"
#include "../core/streamutil.h"
#include "../render/scene/scenegraph.h"
#include "../resources/resources.h"

#include "blueprint/blueprints.h"
#include "object/factory.h"
#include "script/routines.h"
#include "script/util.h"
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

Game::Game(const fs::path &path, const Options &opts) :
    _path(path),
    _opts(opts),
    _window(opts.graphics, this),
    _console(opts.graphics) {

    initGameVersion();
    initObjectFactory();
}

void Game::initGameVersion() {
    fs::path exePath(getPathIgnoreCase(_path, "swkotor2.exe"));
    _version = exePath.empty() ? GameVersion::KotOR : GameVersion::TheSithLords;
}

void Game::initObjectFactory() {
    _objectFactory = make_unique<ObjectFactory>(_version, _opts);
}

int Game::run() {
    _window.init();

    Resources.init(_version, _path);
    TheAudioPlayer.init(_opts.audio);
    RoutineMan.init(_version, this);

    configure();
    loadCursor();

    _console.load();
    _window.show();

    runMainLoop();

    TheJobExecutor.deinit();
    RoutineMan.deinit();
    TheAudioPlayer.deinit();
    Resources.deinit();

    _window.deinit();

    return 0;
}

void Game::configure() {
    loadMainMenu();
    _screen = Screen::MainMenu;

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
    mainMenu->setOnNewGame([this]() {
        _mainMenu->resetFocus();
        if (!_classesGui) loadClassSelectionGui();

        if (_music) {
            _music->stop();
        }
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
    unique_ptr<ClassSelectionGui> gui(new ClassSelectionGui(_objectFactory.get(), _opts.graphics));
    gui->load(_version);
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
    Resources.loadModule(name);
    Blueprints.clearCache();

    shared_ptr<GffStruct> ifo(Resources.findGFF("module", ResourceType::ModuleInfo));

    _module = _objectFactory->newModule();
    configureModule();

    _module->load(name, *ifo);
    _module->loadParty(party, entry);
    _module->area().loadState(_state);

    if (_music) {
        _music->stop();
    }
    string musicName(_module->area().music());
    if (!musicName.empty()) {
        _music = playMusic(musicName);
    }

    if (!_hud) loadHUD();
    if (!_debugOverlay) loadDebugOverlay();
    if (!_dialogGui) loadDialogGui();
    if (!_targetOverlay) loadTargetOverlay();

    _ticks = SDL_GetTicks();
    _screen = Screen::InGame;
}

void Game::loadHUD() {
    unique_ptr<HUD> hud(new HUD(_opts.graphics));
    hud->load(_version);
    hud->setOnEquipmentClick([this]() {
        if (!_equipmentGui) loadEquipmentGui();

        _hud->resetFocus();

        shared_ptr<SpatialObject> player(_module->area().player());
        _equipmentGui->open(player.get());

        _screen = Screen::Equipment;
    });

    _hud = move(hud);
}

void Game::loadDebugOverlay() {
    unique_ptr<DebugOverlay> debug(new DebugOverlay(_opts.graphics));
    debug->load();
    _debugOverlay = move(debug);
}

void Game::loadDialogGui() {
    unique_ptr<DialogGui> dialog(new DialogGui(_opts.graphics));
    dialog->load(_version);
    dialog->setPickReplyEnabled(_pickDialogReplyEnabled);
    dialog->setGetObjectIdByTagFunc([this](const string &tag) {
        shared_ptr<Object> object(_module->area().find(tag));
        return object ? object->id() : 0;
    });
    dialog->setOnReplyPicked(bind(&Game::onDialogReplyPicked, this, _1));
    dialog->setOnSpeakerChanged(bind(&Game::onDialogSpeakerChanged, this, _1, _2));
    dialog->setOnDialogFinished(bind(&Game::onDialogFinished, this));
    _dialogGui = move(dialog);
}

void Game::loadContainerGui() {
    unique_ptr<ContainerGui> container(new ContainerGui(_opts.graphics));
    container->load(_version);
    container->setOnGetItems([this]() {
        shared_ptr<SpatialObject> player(_module->area().player());

        SpatialObject &container = _containerGui->container();
        container.moveItemsTo(*player);

        Placeable *placeable = dynamic_cast<Placeable *>(&container);
        if (placeable) {
            string script;
            if (placeable->blueprint().getScript(PlaceableBlueprint::ScriptType::OnInvDisturbed, script)) {
                runScript(script, placeable->id(), player->id(), -1);
            }
        }
        _screen = Screen::InGame;
    });
    container->setOnClose([this]() {
        _screen = Screen::InGame;
    });
    _containerGui = move(container);
}

void Game::onDialogReplyPicked(uint32_t index) {
}

void Game::onDialogSpeakerChanged(uint32_t from, uint32_t to) {
    shared_ptr<SpatialObject> player(_module->area().player());
    shared_ptr<SpatialObject> partyLeader(_module->area().partyLeader());
    shared_ptr<SpatialObject> prevSpeaker = from != 0 ? _module->area().find(from) : nullptr;
    shared_ptr<SpatialObject> speaker = to != 0 ? _module->area().find(to) : nullptr;
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

void Game::configureModule() {
    _module->setOnCameraChanged([this](CameraType type) {
        _window.setRelativeMouseMode(type == CameraType::FirstPerson);
    });
    _module->setOnModuleTransition([this](const string &name, const string &entry) {
        _nextModule = name;
        _nextEntry = entry;
    });
    _module->setStartDialog([this](const Object &owner, const string &resRef) {
        startDialog(owner.id(), resRef);
    });
    _module->setOpenContainer([this](SpatialObject *object) {
        if (!_containerGui) loadContainerGui();

        _containerGui->open(object);
        _screen = Screen::Container;
    });
}

void Game::startDialog(uint32_t ownerId, const string &resRef) {
    _screen = Screen::Dialog;
    _dialogGui->startDialog(ownerId, resRef);
}

void Game::loadCursor() {
    Cursor cursor;
    cursor.pressed = Resources.findTexture("gui_mp_defaultd", TextureType::Cursor);
    cursor.unpressed = Resources.findTexture("gui_mp_defaultu", TextureType::Cursor);

    _window.setCursor(cursor);
}

void Game::loadEquipmentGui() {
    unique_ptr<EquipmentGui> equip(new EquipmentGui(_opts.graphics));
    equip->load(_version);
    equip->setOnClose([this]() {
        _equipmentGui->resetFocus();
        _screen = Screen::InGame;
    });
    _equipmentGui = move(equip);
}

void Game::loadTargetOverlay() {
    unique_ptr<TargetOverlay> overlay(new TargetOverlay(_opts.graphics));
    overlay->load();
    _targetOverlay = move(overlay);
}

void Game::runMainLoop() {
    _ticks = SDL_GetTicks();

    while (!_quit) {
        _window.processEvents(_quit);
        update();

        _window.clear();
        drawWorld();
        drawGUI();
        drawGUI3D();
        drawCursor();

        _window.swapBuffers();
    }
}

void Game::update() {
    if (!_nextModule.empty()) {
        loadNextModule();
    }
    float dt = getDeltaTime();
    _window.update(dt);

    shared_ptr<GUI> gui(currentGUI());
    if (gui) {
        gui->update(dt);
    }
    bool updModule = _screen == Screen::InGame || _screen == Screen::Dialog;
    if (updModule && _module) {
        GuiContext guiCtx;
        _module->update(dt, guiCtx);

        if (_module->cameraType() == CameraType::ThirdPerson) {
            _hud->update(guiCtx.hud);
        }
        _targetOverlay->setContext(guiCtx.target);
        _debugOverlay->update(guiCtx.debug);
    }

}

void Game::loadNextModule() {
    info("Awaiting async jobs completion");
    JobExecutor &jobs = TheJobExecutor;
    jobs.cancel();
    jobs.await();

    if (_module) {
        _module->saveTo(_state);
    }
    TheSceneGraph.clear();
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
        case Screen::Container:
            return _containerGui;
        case Screen::Equipment:
            return _equipmentGui;
        default:
            return nullptr;
    }
}

void Game::drawWorld() {
    shared_ptr<Camera> camera(_module ? _module->getCamera() : nullptr);
    if (!camera) return;

    glEnable(GL_DEPTH_TEST);

    ShaderUniforms uniforms;
    uniforms.projection = camera->projection();
    uniforms.view = camera->view();
    uniforms.cameraPosition = camera->position();

    Shaders.setGlobalUniforms(uniforms);

    switch (_screen) {
        case Screen::InGame:
        case Screen::Dialog:
        case Screen::Container:
            TheSceneGraph.render();
            break;
        default:
            break;
    }
}

void Game::drawGUI() {
    glDisable(GL_DEPTH_TEST);

    ShaderUniforms uniforms;
    uniforms.projection = glm::ortho(0.0f, static_cast<float>(_opts.graphics.width), static_cast<float>(_opts.graphics.height), 0.0f);

    Shaders.setGlobalUniforms(uniforms);

    switch (_screen) {
        case Screen::InGame:
            _targetOverlay->render();
            _debugOverlay->render();

            if (_module->cameraType() == CameraType::ThirdPerson) {
                _hud->render();
            }
            if (_console.isOpen()) {
                _console.render();
            }
            break;

        default: {
            shared_ptr<GUI> gui(currentGUI());
            if (gui) gui->render();
            break;
        }
    }
}

void Game::drawGUI3D() {
    glEnable(GL_DEPTH_TEST);

    ShaderUniforms uniforms;
    uniforms.projection = glm::ortho(
        0.0f,
        static_cast<float>(_opts.graphics.width),
        static_cast<float>(_opts.graphics.height),
        0.0f,
        -1024.0f,
        1024.0f);

    Shaders.setGlobalUniforms(uniforms);

    switch (_screen) {
        case Screen::MainMenu:
        case Screen::ClassSelection:
        case Screen::PortraitSelection:
            currentGUI()->render3D();
            break;
        default:
            break;
    }
}

void Game::drawCursor() {
    glDisable(GL_DEPTH_TEST);

    ShaderUniforms uniforms;
    uniforms.projection = glm::ortho(0.0f, static_cast<float>(_opts.graphics.width), static_cast<float>(_opts.graphics.height), 0.0f);

    Shaders.setGlobalUniforms(uniforms);

    _window.drawCursor();
}

bool Game::handle(const SDL_Event &event) {
    switch (_screen) {
        case Screen::InGame:
            if (_console.handle(event)) return true;
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

Module *Game::getModule() {
    return _module.get();
}

Area *Game::getArea() {
    return &_module->area();
}

shared_ptr<Object> Game::getObjectById(uint32_t id) {
    return _module->area().find(id);
}

shared_ptr<Object> Game::getObjectByTag(const string &tag, int nth) {
    return _module->area().find(tag, nth);
}

shared_ptr<Object> Game::getWaypointByTag(const string &tag) {
    return _module->area().find(tag);
}

shared_ptr<Object> Game::getPlayer() {
    return _module->area().player();
}

int Game::eventUserDefined(int eventNumber) {
    return _module->area().eventUserDefined(eventNumber);
}

void Game::signalEvent(uint32_t objectId, int eventId) {
    assert(objectId >= 2);
    Area &area = _module->area();
    if (objectId != area.id()) {
        warn("Game: event object is not an area");
        return;
    }
    area.signalEvent(eventId);
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

} // namespace game

} // namespace reone
