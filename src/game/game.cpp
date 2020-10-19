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

#include "../system/audio/player.h"
#include "../system/audio/util.h"
#include "../system/jobs.h"
#include "../system/log.h"
#include "../system/pathutil.h"
#include "../system/streamutil.h"
#include "../system/render/scene/scenegraph.h"
#include "../system/resource/resources.h"

#include "object/objectfactory.h"
#include "script/routines.h"
#include "script/util.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
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
    _options(opts),
    _window(opts.graphics, this),
    _sceneGraph(opts.graphics),
    _worldPipeline(&_sceneGraph, opts.graphics),
    _console(opts.graphics) {

    initGameVersion();
    initObjectFactory();
}

void Game::initGameVersion() {
    fs::path exePath(getPathIgnoreCase(_path, "swkotor2.exe", false));
    _version = exePath.empty() ? GameVersion::KotOR : GameVersion::TheSithLords;
}

void Game::initObjectFactory() {
    _objectFactory = make_unique<ObjectFactory>(_version, &_sceneGraph, _options.graphics);
}

int Game::run() {
    initSubsystems();
    loadResources();
    openMainMenu();
    configure();

    _window.show();

    runMainLoop();
    deinitSubsystems();

    return 0;
}

void Game::initSubsystems() {
    _window.init();
    _worldPipeline.init();

    Resources.init(_version, _path);
    TheAudioPlayer.init(_options.audio);
    Routines.init(_version, this);
}

void Game::loadResources() {
    loadCursor();
    _console.load();
}

void Game::loadCursor() {
    ResourceManager &resources = Resources;

    Cursor cursor;
    cursor.pressed = resources.findTexture("gui_mp_defaultd", TextureType::Cursor);
    cursor.unpressed = resources.findTexture("gui_mp_defaultu", TextureType::Cursor);

    _window.setCursor(cursor);
}

void Game::openMainMenu() {
    if (!_mainMenu) {
        loadMainMenu();
    }
    _screen = GameScreen::MainMenu;

    if (_music) {
        _music->stop();
    }
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
    _mainMenu.reset(new MainMenu(_version, _options.graphics));
    _mainMenu->load();
    _mainMenu->setOnNewGame(bind(&Game::onNewGame, this));
    _mainMenu->setOnExit([this]() { _quit = true; });
    _mainMenu->setOnModuleSelected(bind(&Game::onModuleSelected, this, _1));
}

void Game::onNewGame() {
    _mainMenu->resetFocus();

    if (!_loadScreen) {
        loadLoadingScreen();
    }
    withLoadingScreen([this]() {
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
        if (!_charGen) {
            loadCharacterGeneration();
        }
        _screen = GameScreen::CharacterGeneration;
    });
}

void Game::onModuleSelected(const string &name) {
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
}


void Game::loadModule(const string &name, const PartyConfiguration &party, string entry) {
    info("Game: load module: " + name);

    withLoadingScreen([this, &name, &party, &entry]() {
        ResourceManager &resources = Resources;
        resources.loadModule(name);

        shared_ptr<GffStruct> ifo(resources.findGFF("module", ResourceType::ModuleInfo));
        _module = _objectFactory->newModule();
        configureModule();
        _module->load(name, *ifo);
        _module->area()->setOnCameraChanged([this](CameraType type) {
            _window.setRelativeMouseMode(type == CameraType::FirstPerson);
        });
        _module->loadParty(party, entry);
        _module->area()->loadState(_state);

        if (_music) {
            _music->stop();
        }
        string musicName(_module->area()->music());
        if (!musicName.empty()) {
            _music = playMusic(musicName);
        }
        if (!_hud) {
            loadHUD();
        }
        if (!_dialogGui) {
            loadDialogGui();
        }
        if (!_containerGui) {
            loadContainerGui();
        }
        if (!_equipmentGui) {
            loadEquipmentGui();
        }
        _ticks = SDL_GetTicks();
        _screen = GameScreen::InGame;
    });
}

void Game::withLoadingScreen(const function<void()> &block) {
    _screen = GameScreen::Loading;
    drawAll();
    block();
}

void Game::drawAll() {
    _window.clear();

    drawWorld();
    drawGUI();
    drawCursor();

    _window.swapBuffers();
}

void Game::drawWorld() {
    shared_ptr<CameraSceneNode> cameraNode;

    switch (_screen) {
        case GameScreen::InGame:
        case GameScreen::Container: {
            Camera *camera = _module ? _module->area()->getCamera() : nullptr;
            cameraNode = camera ? camera->sceneNode() : nullptr;
            break;
        }
        case GameScreen::Dialog:
            cameraNode = _dialogGui->camera().sceneNode();
            break;
        default:
            break;
    }
    if (!cameraNode) return;

    _sceneGraph.setActiveCamera(cameraNode);
    _worldPipeline.render();
}

void Game::drawGUI() {
    glDisable(GL_DEPTH_TEST);

    GlobalUniforms globals;
    globals.projection = glm::ortho(
        0.0f,
        static_cast<float>(_options.graphics.width),
        static_cast<float>(_options.graphics.height),
        0.0f,
        -100.0f, 100.0f);

    Shaders.setGlobalUniforms(globals);

    switch (_screen) {
        case GameScreen::InGame:
            if (_module->area()->cameraType() == CameraType::ThirdPerson) {
                _hud->render();
            }
            if (_console.isOpen()) {
                _console.render();
            }
            break;

        default: {
            GUI *gui = getScreenGUI();
            if (gui) {
                gui->render();
                gui->render3D();
            }
            break;
        }
    }
}

void Game::drawCursor() {
    glDisable(GL_DEPTH_TEST);

    GlobalUniforms globals;
    globals.projection = glm::ortho(0.0f, static_cast<float>(_options.graphics.width), static_cast<float>(_options.graphics.height), 0.0f);
    globals.view = glm::mat4(1.0f);

    Shaders.setGlobalUniforms(globals);

    _window.drawCursor();
}

void Game::configureModule() {
    _module->setOnModuleTransition([this](const string &name, const string &entry) {
        _nextModule = name;
        _nextEntry = entry;
    });
    _module->setStartDialog([this](SpatialObject &owner, const string &resRef) {
        startDialog(owner, resRef);
    });
    _module->setOpenContainer([this](SpatialObject *object) {
        _containerGui->open(object);
        _screen = GameScreen::Container;
    });
}

void Game::startDialog(SpatialObject &owner, const string &resRef) {
    _screen = GameScreen::Dialog;
    _dialogGui->startDialog(owner, resRef);
}

void Game::loadHUD() {
    _hud.reset(new HUD(_version, _options.graphics));
    _hud->load();
    _hud->setOnEquipmentClick(bind(&Game::onEquipmentClick, this));
}

void Game::onEquipmentClick() {
    _hud->resetFocus();

    shared_ptr<SpatialObject> player(_module->area()->player());
    _equipmentGui->open(player.get());

    _screen = GameScreen::Equipment;
}

void Game::loadDialogGui() {
    _dialogGui.reset(new DialogGui(_version, this, _options.graphics));
    _dialogGui->load();
    _dialogGui->setPickReplyEnabled(_pickDialogReplyEnabled);
    _dialogGui->setOnReplyPicked(bind(&Game::onDialogReplyPicked, this, _1));
    _dialogGui->setOnDialogFinished(bind(&Game::onDialogFinished, this));
}

void Game::onDialogReplyPicked(uint32_t index) {
}

void Game::onDialogFinished() {
    _screen = GameScreen::InGame;
}

void Game::loadContainerGui() {
    _containerGui.reset(new ContainerGui(_version, _options.graphics));
    _containerGui->load();
    _containerGui->setOnGetItems(bind(&Game::onGetItems, this));
    _containerGui->setOnClose([this]() { _screen = GameScreen::InGame; });
}

void Game::onGetItems() {
    shared_ptr<SpatialObject> player(_module->area()->player());

    SpatialObject &container = _containerGui->container();
    container.moveItemsTo(*player);

    Placeable *placeable = dynamic_cast<Placeable *>(&container);
    if (placeable) {
        string script;
        if (placeable->blueprint().getScript(PlaceableBlueprint::ScriptType::OnInvDisturbed, script)) {
            runScript(script, placeable->id(), player->id(), -1);
        }
    }
    _screen = GameScreen::InGame;
}

void Game::loadEquipmentGui() {
    _equipmentGui.reset(new EquipmentGui(_version, _options.graphics));
    _equipmentGui->load();
    _equipmentGui->setOnClose([this]() {
        _equipmentGui->resetFocus();
        _screen = GameScreen::InGame;
    });
}

float Game::getDeltaTime() {
    uint32_t ticks = SDL_GetTicks();
    float dt = (ticks - _ticks) / 1000.0f;
    _ticks = ticks;

    return dt;
}

GUI *Game::getScreenGUI() const {
    switch (_screen) {
        case GameScreen::MainMenu:
            return _mainMenu.get();
        case GameScreen::Loading:
            return _loadScreen.get();
        case GameScreen::CharacterGeneration:
            return _charGen.get();
        case GameScreen::InGame:
            return _hud.get();
        case GameScreen::Dialog:
            return _dialogGui.get();
        case GameScreen::Container:
            return _containerGui.get();
        case GameScreen::Equipment:
            return _equipmentGui.get();
        default:
            return nullptr;
    }
}

void Game::configure() {
}

void Game::runMainLoop() {
    _ticks = SDL_GetTicks();

    while (!_quit) {
        _window.processEvents(_quit);

        update();
        drawAll();
    }
}

void Game::update() {
    if (!_nextModule.empty()) {
        loadNextModule();
    }
    float dt = getDeltaTime();
    _window.update(dt);

    GUI *gui = getScreenGUI();
    if (gui) {
        gui->update(dt);
    }
    bool updModule = _screen == GameScreen::InGame || _screen == GameScreen::Dialog;

    if (updModule && _module) {
        GuiContext guiCtx;
        _module->update(dt, guiCtx);

        if (_module->area()->cameraType() == CameraType::ThirdPerson) {
            _hud->setContext(guiCtx);
        }
    }
}

void Game::loadNextModule() {
    JobExecutor &jobs = Jobs;
    jobs.cancel();
    jobs.await();

    if (_module) {
        _module->saveTo(_state);
    }
    _sceneGraph.clear();
    loadModule(_nextModule, _state.party, _nextEntry);

    _nextModule.clear();
    _nextEntry.clear();
}

void Game::loadLoadingScreen() {
    _loadScreen.reset(new LoadingScreen(_version, _options.graphics));
    _loadScreen->load();
}

void Game::loadCharacterGeneration() {
    _charGen.reset(new CharacterGeneration(_version, _options.graphics));
    _charGen->load();
    _charGen->setOnPlay(bind(&Game::onPlay, this, _1));
    _charGen->setOnCancel(bind(&Game::openMainMenu, this));
}

void Game::onPlay(const CreatureConfiguration &config) {
    string moduleName(_version == GameVersion::KotOR ? "end_m01aa" : "001ebo");

    PartyConfiguration party;
    party.memberCount = 1;
    party.leader = config;

    loadModule(moduleName, party);
}

void Game::deinitSubsystems() {
    Jobs.deinit();
    Routines.deinit();
    TheAudioPlayer.deinit();
    Resources.deinit();

    _window.deinit();
}

bool Game::handle(const SDL_Event &event) {
    GUI *gui = getScreenGUI();
    if (gui && gui->handle(event)) {
        return true;
    }
    switch (_screen) {
        case GameScreen::InGame:
            if (_console.handle(event)) {
                return true;
            }
            if (_module->handle(event)) {
                return true;
            }
            break;
        default:
            break;
    }

    return false;
}

int Game::eventUserDefined(int eventNumber) {
    int id = _eventCounter++;

    UserDefinedEvent event { eventNumber };
    _events.insert(make_pair(id, move(event)));

    return id;
}

int Game::getUserDefinedEventNumber(int eventId) {
    auto maybeEvent = _events.find(eventId);
    if (maybeEvent == _events.end()) {
        warn("Event not found by id: " + to_string(eventId));
        return -1;
    }

    return maybeEvent->second.eventNumber;
}

shared_ptr<Module> Game::module() const {
    return _module;
}

bool Game::getGlobalBoolean(const string &name) const {
    auto maybeValue = _state.globalBooleans.find(name);
    return maybeValue != _state.globalBooleans.end() ? maybeValue->second : false;
}

int Game::getGlobalNumber(const string &name) const {
    auto maybeValue = _state.globalNumbers.find(name);
    return maybeValue != _state.globalNumbers.end() ? maybeValue->second : 0;
}

bool Game::getLocalBoolean(uint32_t objectId, int index) const {
    auto maybeObject = _state.localBooleans.find(objectId);
    if (maybeObject == _state.localBooleans.end()) return false;

    auto maybeValue = maybeObject->second.find(index);
    if (maybeValue == maybeObject->second.end()) return false;

    return maybeValue->second;
}

int Game::getLocalNumber(uint32_t objectId, int index) const {
    auto maybeObject = _state.localNumbers.find(objectId);
    if (maybeObject == _state.localNumbers.end()) return 0;

    auto maybeValue = maybeObject->second.find(index);
    if (maybeValue == maybeObject->second.end()) return 0;

    return maybeValue->second;
}

void Game::setGlobalBoolean(const string &name, bool value) {
    _state.globalBooleans[name] = value;
}

void Game::setGlobalNumber(const string &name, int value) {
    _state.globalNumbers[name] = value;
}

void Game::setLocalBoolean(uint32_t objectId, int index, bool value) {
    _state.localBooleans[objectId][index] = value;
}

void Game::setLocalNumber(uint32_t objectId, int index, int value) {
    _state.localBooleans[objectId][index] = value;
}

} // namespace game

} // namespace reone
