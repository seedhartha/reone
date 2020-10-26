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

#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_timer.h"

#include "../audio/files.h"
#include "../audio/player.h"
#include "../audio/util.h"
#include "../render/models.h"
#include "../render/textures.h"
#include "../render/walkmeshes.h"
#include "../resource/resources.h"
#include "../script/scripts.h"
#include "../system/jobs.h"
#include "../system/log.h"
#include "../system/pathutil.h"

#include "blueprints.h"
#include "script/routines.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

Game::Game(const fs::path &path, const Options &opts) :
    _path(path),
    _options(opts),
    _window(opts.graphics, this),
    _sceneGraph(opts.graphics),
    _worldPipeline(&_sceneGraph, opts.graphics),
    _console(opts.graphics) {

    initGameVersion();
    _objectFactory = make_unique<ObjectFactory>(_version, this, &_sceneGraph, _options.graphics);
}

void Game::initGameVersion() {
    fs::path exePath(getPathIgnoreCase(_path, "swkotor2.exe", false));
    _version = exePath.empty() ? GameVersion::KotOR : GameVersion::TheSithLords;
}

int Game::run() {
    init();
    loadResources();
    openMainMenu();

    _window.show();

    runMainLoop();
    deinit();

    return 0;
}

void Game::init() {
    _window.init();
    _worldPipeline.init();

    Resources.init(_version, _path);
    Models::instance().init(_version);
    Textures::instance().init(_version);
    TheAudioPlayer.init(_options.audio);
    Routines.init(_version, this);
}

void Game::loadResources() {
    loadCursor();
    _console.load();
}

void Game::loadCursor() {
    Cursor cursor;
    cursor.pressed = Textures::instance().get("gui_mp_defaultd", TextureType::Cursor);
    cursor.unpressed = Textures::instance().get("gui_mp_defaultu", TextureType::Cursor);

    _window.setCursor(cursor);
}

void Game::openMainMenu() {
    if (!_mainMenu) {
        loadMainMenu();
    }
    playMusic(getMainMenuMusic());
    _screen = GameScreen::MainMenu;
}

string Game::getMainMenuMusic() const {
    switch (_version) {
        case GameVersion::TheSithLords:
            return "mus_sion";
        default:
            return "mus_theme_cult";
    }
}

void Game::playMusic(const string &resRef) {
    if (_music) {
        _music->stop();
    }
    if (!resRef.empty()) {
        _music = ::playMusic(resRef);
    }
}

void Game::loadMainMenu() {
    _mainMenu.reset(new MainMenu(this, _version, _options.graphics));
    _mainMenu->load();
}

void Game::loadModule(const string &name, const PartyConfiguration &party, string entry) {
    info("Game: load module: " + name);
    _partyConfig = party;

    withLoadingScreen([this, &name, &party, &entry]() {
        ResourceManager &resources = Resources;
        resources.loadModule(name);

        Models::instance().invalidateCache();
        Walkmeshes::instance().invalidateCache();
        Textures::instance().invalidateCache();
        AudioFiles::instance().invalidateCache();
        Scripts::instance().invalidateCache();
        Blueprints::instance().invalidateCache();

        shared_ptr<GffStruct> ifo(resources.findGFF("module", ResourceType::ModuleInfo));
        _module = _objectFactory->newModule();
        _module->load(name, *ifo);
        _module->loadParty(_partyConfig, entry);

        string musicName(_module->area()->music());
        playMusic(musicName);

        if (!_hud) {
            loadHUD();
        }
        if (!_dialog) {
            loadDialog();
        }
        if (!_container) {
            loadContainer();
        }
        if (!_equipment) {
            loadEquipment();
        }
        if (!_partySelection) {
            loadPartySelection();
        }
        _ticks = SDL_GetTicks();
        _screen = GameScreen::InGame;
    });
}

void Game::withLoadingScreen(const function<void()> &block) {
    if (!_loadScreen) {
        loadLoadingScreen();
    }
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
            cameraNode = _dialog->camera().sceneNode();
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

void Game::loadHUD() {
    _hud.reset(new HUD(this, _version, _options.graphics));
    _hud->load();
}

void Game::loadDialog() {
    _dialog.reset(new Dialog(this, _version, _options.graphics));
    _dialog->load();
}

void Game::loadContainer() {
    _container.reset(new Container(this, _version, _options.graphics));
    _container->load();
}

void Game::loadEquipment() {
    _equipment.reset(new Equipment(this, _version, _options.graphics));
    _equipment->load();
}

void Game::loadPartySelection() {
    _partySelection.reset(new PartySelection(this, _objectFactory.get(), _version, _options.graphics));
    _partySelection->load();
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
            return _dialog.get();
        case GameScreen::Container:
            return _container.get();
        case GameScreen::Equipment:
            return _equipment.get();
        case GameScreen::PartySelection:
            return _partySelection.get();
        default:
            return nullptr;
    }
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
    float dt = measureFrameTime();
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

    _sceneGraph.clear();
    loadModule(_nextModule, _partyConfig, _nextEntry);

    _nextModule.clear();
    _nextEntry.clear();
}

float Game::measureFrameTime() {
    uint32_t ticks = SDL_GetTicks();
    float dt = (ticks - _ticks) / 1000.0f;
    _ticks = ticks;

    return dt;
}

void Game::loadLoadingScreen() {
    _loadScreen.reset(new LoadingScreen(_version, _options.graphics));
    _loadScreen->load();
}

void Game::loadCharacterGeneration() {
    _charGen.reset(new CharacterGeneration(this, _version, _options.graphics));
    _charGen->load();
}

void Game::deinit() {
    Jobs.deinit();
    Routines.deinit();
    TheAudioPlayer.deinit();
    Resources.deinit();

    _window.deinit();
}

void Game::startCharacterGeneration() {
    withLoadingScreen([this]() {
        if (!_charGen) {
            loadCharacterGeneration();
        }
        playMusic(getCharacterGenerationMusic());
        _screen = GameScreen::CharacterGeneration;
    });
}

string Game::getCharacterGenerationMusic() const {
    switch (_version) {
        case GameVersion::TheSithLords:
            return "mus_main";
        default:
            return "mus_theme_rep";
    }
}

void Game::quit() {
    _quit = true;
}

void Game::openInGame() {
    _screen = GameScreen::InGame;
}

void Game::startDialog(SpatialObject &owner, const string &resRef) {
    _screen = GameScreen::Dialog;
    _dialog->startDialog(owner, resRef);
}

void Game::openContainer(SpatialObject *container) {
    _container->open(container);
    _screen = GameScreen::Container;
}

void Game::openPartySelection(const PartySelection::Context &ctx) {
    _partySelection->prepare(ctx);
    _screen = GameScreen::PartySelection;
}

void Game::scheduleModuleTransition(const string &moduleName, const string &entry) {
    _nextModule = moduleName;
    _nextEntry = entry;
}

void Game::onCameraChanged(CameraType camera) {
    _window.setRelativeMouseMode(camera == CameraType::FirstPerson);
}

void Game::openEquipment() {
    _equipment->update();
    _screen = GameScreen::Equipment;
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

shared_ptr<Module> Game::module() const {
    return _module;
}

Party &Game::party() {
    return _party;
}

bool Game::getGlobalBoolean(const string &name) const {
    auto maybeValue = _globalBooleans.find(name);
    return maybeValue != _globalBooleans.end() ? maybeValue->second : false;
}

int Game::getGlobalNumber(const string &name) const {
    auto maybeValue = _globalNumbers.find(name);
    return maybeValue != _globalNumbers.end() ? maybeValue->second : 0;
}

bool Game::getLocalBoolean(uint32_t objectId, int index) const {
    auto maybeObject = _localBooleans.find(objectId);
    if (maybeObject == _localBooleans.end()) return false;

    auto maybeValue = maybeObject->second.find(index);
    if (maybeValue == maybeObject->second.end()) return false;

    return maybeValue->second;
}

int Game::getLocalNumber(uint32_t objectId, int index) const {
    auto maybeObject = _localNumbers.find(objectId);
    if (maybeObject == _localNumbers.end()) return 0;

    auto maybeValue = maybeObject->second.find(index);
    if (maybeValue == maybeObject->second.end()) return 0;

    return maybeValue->second;
}

void Game::setGlobalBoolean(const string &name, bool value) {
    _globalBooleans[name] = value;
}

void Game::setGlobalNumber(const string &name, int value) {
    _globalNumbers[name] = value;
}

void Game::setLocalBoolean(uint32_t objectId, int index, bool value) {
    _localBooleans[objectId][index] = value;
}

void Game::setLocalNumber(uint32_t objectId, int index, int value) {
    _localNumbers[objectId][index] = value;
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
        warn("Game: event not found by id: " + to_string(eventId));
        return -1;
    }

    return maybeEvent->second.eventNumber;
}

} // namespace game

} // namespace reone
