/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "SDL2/SDL_timer.h"

#include "../audio/files.h"
#include "../audio/player.h"
#include "../audio/util.h"
#include "../render/models.h"
#include "../render/textures.h"
#include "../render/walkmeshes.h"
#include "../resource/resources.h"
#include "../script/scripts.h"
#include "../common/jobs.h"
#include "../common/log.h"
#include "../common/pathutil.h"
#include "../video/bikfile.h"
#include "../video/video.h"

#include "blueprint/blueprints.h"
#include "cursors.h"
#include "script/routines.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;
using namespace reone::video;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

Game::Game(const fs::path &path, const Options &opts) :
    _path(path),
    _options(opts),
    _window(opts.graphics, this),
    _sceneGraph(opts.graphics),
    _worldPipeline(&_sceneGraph, opts.graphics),
    _console(opts.graphics),
    _party(this) {

    initGameVersion();
    _objectFactory = make_unique<ObjectFactory>(this, &_sceneGraph);
}

void Game::initGameVersion() {
    fs::path exePath(getPathIgnoreCase(_path, "swkotor2.exe", false));
    _version = exePath.empty() ? GameVersion::KotOR : GameVersion::TheSithLords;
}

int Game::run() {
    init();
    openMainMenu();

    if (!_options.module.empty()) {
        _mainMenu->onModuleSelected(_options.module);
    } else {
        playVideo("legal");
    }
    _window.show();

    runMainLoop();
    deinit();

    return 0;
}

void Game::init() {
    _window.init();
    _worldPipeline.init();

    Resources::instance().init(_version, _path);
    Cursors::instance().init(_version);
    Models::instance().init(_version);
    Textures::instance().init(_version);
    AudioPlayer::instance().init(_options.audio);
    Routines::instance().init(_version, this);

    setCursorType(CursorType::Default);

    _console.load();
}

void Game::setCursorType(CursorType type) {
    if (_cursorType == type) return;

    _cursorType = type;

    if (type == CursorType::None) {
        _window.setCursor(nullptr);
        return;
    }
    shared_ptr<Cursor> cursor(Cursors::instance().get(type));
    _window.setCursor(cursor);
}

void Game::playVideo(const string &name) {
    fs::path path(getPathIgnoreCase(_path, "movies/" + name + ".bik"));

    BikFile bik(path);
    bik.load();

    _video = bik.video();

    if (_music && !_music->isStopped()) {
        _music->stop();
    }
    shared_ptr<AudioStream> audio(_video->audio());
    if (audio) {
        _movieAudio = AudioPlayer::instance().play(audio, AudioType::Movie);
    }
}

void Game::openMainMenu() {
    if (!_mainMenu) {
        loadMainMenu();
    }
    if (!_saveLoad) {
        loadSaveLoad();
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
    if (_musicResRef == resRef) return;

    if (_music && !_music->isStopped()) {
        _music->stop();
    }
    _musicResRef = resRef;
}

void Game::loadMainMenu() {
    _mainMenu.reset(new MainMenu(this));
    _mainMenu->load();
}

void Game::loadModule(const string &name, const string &entry) {
    info("Game: load module: " + name);

    withLoadingScreen([this, &name, &entry]() {
        if (!_hud) {
            loadHUD();
        }
        if (!_inGame) {
            loadInGame();
        }
        if (!_dialog) {
            loadDialog();
        }
        if (!_container) {
            loadContainer();
        }
        if (!_partySelect) {
            loadPartySelection();
        }

        Models::instance().invalidateCache();
        Walkmeshes::instance().invalidateCache();
        Textures::instance().invalidateCache();
        AudioFiles::instance().invalidateCache();
        Scripts::instance().invalidateCache();
        Blueprints::instance().invalidateCache();
        Resources::instance().loadModule(name);

        if (_module) {
            _module->area()->unloadParty();
        }

        auto maybeModule = _loadedModules.find(name);
        if (maybeModule != _loadedModules.end()) {
            _module = maybeModule->second;
        } else {
            shared_ptr<GffStruct> ifo(Resources::instance().getGFF("module", ResourceType::ModuleInfo));

            _module = _objectFactory->newModule();
            _module->load(name, *ifo);

            _loadedModules.insert(make_pair(name, _module));
        }

        _module->loadParty(entry);
        _module->area()->fill(_sceneGraph);

        string musicName(_module->area()->music());
        playMusic(musicName);

        _ticks = SDL_GetTicks();
        _screen = GameScreen::InGame;
        _loadFromSaveGame = false;
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

    if (_video) {
        _video->render();
    } else {
        drawWorld();
        drawGUI();
        drawCursor();
    }

    _window.swapBuffers();
}

void Game::drawWorld() {
    const Camera *camera = getActiveCamera();
    if (!camera) return;

    _sceneGraph.setActiveCamera(camera->sceneNode());
    _sceneGraph.prepareFrame();

    _worldPipeline.render();
}

Camera *Game::getActiveCamera() const {
    switch (_screen) {
        case GameScreen::InGame:
        case GameScreen::Container: {
            shared_ptr<Area> area(_module->area());
            CameraType type = area->cameraType();
            Camera *camera = type == CameraType::ThirdPerson ? (Camera *) &area->thirdPersonCamera() : &area->firstPersonCamera();
            return camera;
        }
        case GameScreen::Dialog:
            return &_dialog->camera();
        default:
            return nullptr;
    }
}

void Game::drawGUI() {
    GlobalUniforms globals;
    globals.projection = glm::ortho(
        0.0f,
        static_cast<float>(_options.graphics.width),
        static_cast<float>(_options.graphics.height),
        0.0f,
        -100.0f, 100.0f);

    Shaders::instance().setGlobalUniforms(globals);

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
    GlobalUniforms globals;
    globals.projection = glm::ortho(0.0f, static_cast<float>(_options.graphics.width), static_cast<float>(_options.graphics.height), 0.0f);
    globals.view = glm::mat4(1.0f);

    Shaders::instance().setGlobalUniforms(globals);

    _window.drawCursor();
}

void Game::loadHUD() {
    _hud.reset(new HUD(this));
    _hud->load();
}

void Game::loadDialog() {
    _dialog.reset(new DialogGUI(this));
    _dialog->load();
}

void Game::loadContainer() {
    _container.reset(new Container(this));
    _container->load();
}

void Game::loadPartySelection() {
    _partySelect.reset(new PartySelection(this));
    _partySelect->load();
}

void Game::loadSaveLoad() {
    _saveLoad.reset(new SaveLoad(this));
    _saveLoad->load();
}

void Game::loadInGame() {
    _inGame.reset(new InGameMenu(this));
    _inGame->load();
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
        case GameScreen::InGameMenu:
            return _inGame.get();
        case GameScreen::Dialog:
            return _dialog.get();
        case GameScreen::Container:
            return _container.get();
        case GameScreen::PartySelection:
            return _partySelect.get();
        case GameScreen::SaveLoad:
            return _saveLoad.get();
        default:
            return nullptr;
    }
}

bool Game::isLoadFromSaveGame() const {
    return _loadFromSaveGame;
}

void Game::setLoadFromSaveGame(bool load) {
    _loadFromSaveGame = load;
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
    float dt = measureFrameTime();

    if (_video) {
        _video->update(dt);
        if (_video->isFinished()) {
            _video.reset();
        }
    } else if (!_musicResRef.empty() && (!_music || _music->isStopped())) {
        _music = ::playMusic(_musicResRef);
    }

    if (!_nextModule.empty()) {
        loadNextModule();
    }

    Camera *camera = getActiveCamera();
    if (camera) {
        camera->update(dt);
    }

    bool updModule = _module && (_screen == GameScreen::InGame || _screen == GameScreen::Dialog);
    if (updModule) {
        _module->update(dt);
    }

    GUI *gui = getScreenGUI();
    if (gui) {
        gui->update(dt);
    }

    _window.update(dt);
}

void Game::loadNextModule() {
    JobExecutor::instance().cancel();
    JobExecutor::instance().await();

    loadModule(_nextModule, _nextEntry);

    _nextModule.clear();
    _nextEntry.clear();
}

float Game::measureFrameTime() {
    uint32_t ticks = SDL_GetTicks();
    float dt = (ticks - _ticks) / 1000.0f;
    _ticks = ticks;

    return dt * _gameSpeed;
}

void Game::loadLoadingScreen() {
    _loadScreen.reset(new LoadingScreen(_version, _options.graphics));
    _loadScreen->load();
}

void Game::loadCharacterGeneration() {
    _charGen.reset(new CharacterGeneration(this));
    _charGen->load();
}

void Game::deinit() {
    JobExecutor::instance().deinit();
    Routines::instance().deinit();
    AudioPlayer::instance().deinit();
    Cursors::instance().deinit();
    Resources::instance().deinit();

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

void Game::openInGameMenu(InGameMenu::Tab tab) {
    setCursorType(CursorType::Default);
    switch (tab) {
        case InGameMenu::Tab::Equipment:
            _inGame->openEquipment();
            break;
        case InGameMenu::Tab::Inventory:
            _inGame->openInventory();
            break;
        case InGameMenu::Tab::Character:
            _inGame->openCharacter();
            break;
        case InGameMenu::Tab::Abilities:
            _inGame->openAbilities();
            break;
        case InGameMenu::Tab::Messages:
            _inGame->openMessages();
            break;
        case InGameMenu::Tab::Journal:
            _inGame->openJournal();
            break;
        case InGameMenu::Tab::Map:
            _inGame->openMap();
            break;
        case InGameMenu::Tab::Options:
            _inGame->openOptions();
            break;
        default:
            break;
    }
    _screen = GameScreen::InGameMenu;
}

void Game::startDialog(SpatialObject &owner, const string &resRef) {
    setCursorType(CursorType::Default);
    _screen = GameScreen::Dialog;
    _dialog->startDialog(owner, resRef);
}

void Game::openContainer(SpatialObject *container) {
    setCursorType(CursorType::Default);
    _container->open(container);
    _screen = GameScreen::Container;
}

void Game::openPartySelection(const PartySelection::Context &ctx) {
    setCursorType(CursorType::Default);
    _partySelect->prepare(ctx);
    _screen = GameScreen::PartySelection;
}

void Game::openSaveLoad(SaveLoad::Mode mode) {
    setCursorType(CursorType::Default);
    _saveLoad->setMode(mode);
    _saveLoad->update();
    _screen = GameScreen::SaveLoad;
}

void Game::scheduleModuleTransition(const string &moduleName, const string &entry) {
    _nextModule = moduleName;
    _nextEntry = entry;
}

void Game::onCameraChanged(CameraType camera) {
    _window.setRelativeMouseMode(camera == CameraType::FirstPerson);
}

bool Game::handle(const SDL_Event &event) {
    if (!_video) {
        GUI *gui = getScreenGUI();
        if (gui && gui->handle(event)) {
            return true;
        }
        switch (_screen) {
            case GameScreen::InGame: {
                if (_console.handle(event)) {
                    return true;
                }
                if (_party.handle(event)) {
                    return true;
                }
                Camera *camera = getActiveCamera();
                if (camera && camera->handle(event)) {
                    return true;
                }
                if (_module->handle(event)) {
                    return true;
                }
                break;
            }
            default:
                break;
        }
    }
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (handleMouseButtonDown(event.button)) return true;
            break;
        case SDL_KEYDOWN:
            if (handleKeyDown(event.key)) return true;
            break;
        default:
            break;
    }

    return false;
}

bool Game::handleMouseButtonDown(const SDL_MouseButtonEvent &event) {
    if (_video) {
        _video->finish();
        return true;
    }
    return false;
}

bool Game::handleKeyDown(const SDL_KeyboardEvent &event) {
    if (event.repeat) return false;

    switch (event.keysym.sym) {
        case SDLK_MINUS:
            if (_gameSpeed > 1.0f) {
                _gameSpeed = glm::max(1.0f, _gameSpeed - 1.0f);
            }
            return true;

        case SDLK_EQUALS:
            if (_gameSpeed < 4.0f) {
                _gameSpeed = glm::min(4.0f, _gameSpeed + 1.0f);
            }
            return true;
    }

    return false;
}

GameVersion Game::version() const {
    return _version;
}

const Options &Game::options() const {
    return _options;
}

SceneGraph &Game::sceneGraph() {
    return _sceneGraph;
}

ObjectFactory &Game::objectFactory() {
    return *_objectFactory;
}

shared_ptr<Module> Game::module() const {
    return _module;
}

Party &Game::party() {
    return _party;
}

CharacterGeneration &Game::characterGeneration() {
    return *_charGen;
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
