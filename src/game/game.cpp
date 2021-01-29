/*
 * Copyright (c) 2020-2021 The reone project contributors
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
#include "../audio/soundhandle.h"
#include "../common/jobs.h"
#include "../common/log.h"
#include "../common/pathutil.h"
#include "../render/models.h"
#include "../render/textures.h"
#include "../render/walkmeshes.h"
#include "../resource/resources.h"
#include "../script/scripts.h"
#include "../video/bikfile.h"
#include "../video/video.h"

#include "blueprint/blueprints.h"
#include "cursors.h"
#include "gui/sounds.h"
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
    _console(this),
    _party(this),
    _scriptRunner(this) {

    determineGameID();

    _objectFactory = make_unique<ObjectFactory>(this, &_sceneGraph);
}

void Game::determineGameID() {
    fs::path exePath(getPathIgnoreCase(_path, "swkotor2.exe", false));
    _gameId = exePath.empty() ? GameID::KotOR : GameID::TSL;
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

    Resources::instance().init(_gameId, _path);
    Cursors::instance().init(_gameId);
    Models::instance().init(_gameId);
    Textures::instance().init(_gameId);
    AudioPlayer::instance().init(_options.audio);
    GUISounds::instance().init();
    Routines::instance().init(_gameId, this);

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

    if (_music) {
        _music->stop();
        _music.reset();
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
    changeScreen(GameScreen::MainMenu);
}

void Game::changeScreen(GameScreen screen) {
    GUI *gui = getScreenGUI();
    if (gui) {
        gui->resetFocus();
    }
    _screen = screen;
}

string Game::getMainMenuMusic() const {
    switch (_gameId) {
        case GameID::TSL:
            return "mus_sion";
        default:
            return "mus_theme_cult";
    }
}

void Game::playMusic(const string &resRef) {
    if (_musicResRef == resRef) return;

    if (_music) {
        _music->stop();
        _music.reset();
    }
    _musicResRef = resRef;
}

void Game::loadMainMenu() {
    _mainMenu = make_unique<MainMenu>(this);
    _mainMenu->load();
}

void Game::loadModule(const string &name, string entry) {
    info("Game: load module: " + name);

    withLoadingScreen("load_" + name, [this, &name, &entry]() {
        if (!_hud) {
            loadHUD();
        }
        if (!_inGame) {
            loadInGame();
        }
        if (!_dialog) {
            loadDialog();
        }
        if (!_computer) {
            loadComputer();
        }
        if (!_container) {
            loadContainer();
        }
        if (!_partySelect) {
            loadPartySelection();
        }
        if (!_charGen) {
            loadCharacterGeneration();
        }

        Models::instance().invalidateCache();
        Walkmeshes::instance().invalidateCache();
        Textures::instance().invalidateCache();
        AudioFiles::instance().invalidateCache();
        Scripts::instance().invalidateCache();
        Blueprints::instance().invalidateCache();
        Resources::instance().loadModule(name);

        if (_module) {
            _module->area()->runOnExitScript();
            _module->area()->unloadParty();
        }

        auto maybeModule = _loadedModules.find(name);
        if (maybeModule != _loadedModules.end()) {
            _module = maybeModule->second;
        } else {
            shared_ptr<GffStruct> ifo(Resources::instance().getGFF("module", ResourceType::Ifo));

            _module = _objectFactory->newModule();
            _module->load(name, *ifo);

            _loadedModules.insert(make_pair(name, _module));
        }

        _module->loadParty(entry);
        _module->area()->fill(_sceneGraph);

        string musicName(_module->area()->music());
        playMusic(musicName);

        _ticks = SDL_GetTicks();
        openInGame();
        _loadFromSaveGame = false;
    });
}

void Game::withLoadingScreen(const string &imageResRef, const function<void()> &block) {
    if (!_loadScreen) {
        loadLoadingScreen();
    }
    _loadScreen->setImage(imageResRef);
    changeScreen(GameScreen::Loading);
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
    _sceneGraph.setReferenceNode(_party.getLeader()->model());
    _sceneGraph.prepareFrame();

    _worldPipeline.render();
}

void Game::toggleInGameCameraType() {
    switch (_cameraType) {
        case CameraType::FirstPerson:
            if (_party.getLeader()) {
                _cameraType = CameraType::ThirdPerson;
            }
            break;
        case CameraType::ThirdPerson: {
            _module->player().stopMovement();
            shared_ptr<Area> area(_module->area());
            FirstPersonCamera &firstPerson = static_cast<FirstPersonCamera &>(area->getCamera(CameraType::FirstPerson));
            ThirdPersonCamera &thirdPerson = static_cast<ThirdPersonCamera &>(area->getCamera(CameraType::ThirdPerson));
            firstPerson.setPosition(thirdPerson.sceneNode()->absoluteTransform()[3]);
            firstPerson.setFacing(thirdPerson.facing());
            _cameraType = CameraType::FirstPerson;
            break;
        }
        default:
            break;
    }
    _window.setRelativeMouseMode(_cameraType == CameraType::FirstPerson);
}

Camera *Game::getActiveCamera() const {
    if (!_module) return nullptr;

    shared_ptr<Area> area(_module->area());

    return &area->getCamera(_cameraType);
}

int Game::getRunScriptVar() const {
    return _runScriptVar;
}

shared_ptr<Object> Game::getObjectById(uint32_t id) const {
    switch (id) {
        case kObjectSelf:
            throw invalid_argument("id is invalid");
        case kObjectInvalid:
            return nullptr;
        default:
            break;
    }

    if (_module->id() == id) return _module;

    auto area = _module->area();
    if (area->id() == id) return area;

    return area->getObjectById(id);
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
            if (_cameraType == CameraType::ThirdPerson) {
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
    _hud = make_unique<HUD>(this);
    _hud->load();
}

void Game::loadDialog() {
    _dialog = make_unique<DialogGUI>(this);
    _dialog->load();
}

void Game::loadComputer() {
    _computer = make_unique<ComputerGUI>(this);
    _computer->load();
}

void Game::loadContainer() {
    _container = make_unique<Container>(this);
    _container->load();
}

void Game::loadPartySelection() {
    _partySelect = make_unique<PartySelection>(this);
    _partySelect->load();
}

void Game::loadSaveLoad() {
    _saveLoad = make_unique<SaveLoad>(this);
    _saveLoad->load();
}

void Game::loadInGame() {
    _inGame = make_unique<InGameMenu>(this);
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
        case GameScreen::Conversation:
            return _conversation;
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

        this_thread::yield();
    }
}

void Game::update() {
    float dt = measureFrameTime();

    if (_video) {
        updateVideo(dt);
    } else {
        updateMusic();
    }
    if (!_nextModule.empty()) {
        loadNextModule();
    }
    updateCamera(dt);

    bool updModule = !_video && _module && (_screen == GameScreen::InGame || _screen == GameScreen::Conversation);
    if (updModule) {
        _module->update(dt);
    }

    GUI *gui = getScreenGUI();
    if (gui) {
        gui->update(dt);
    }

    _window.update(dt);

    if (!_paused) {
        _sceneGraph.update(dt);
    }
}

void Game::updateVideo(float dt) {
    _video->update(dt);

    if (_video->isFinished()) {
        if (_movieAudio) {
            _movieAudio->stop();
            _movieAudio.reset();
        }
        _video.reset();
    }
}

void Game::updateMusic() {
    if (_musicResRef.empty()) return;

    if (!_music || _music->isStopped()) {
        _music = AudioPlayer::instance().play(_musicResRef, AudioType::Music);
    }
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
    _loadScreen = make_unique<LoadingScreen>(this);
    _loadScreen->load();
}

void Game::loadCharacterGeneration() {
    _charGen = make_unique<CharacterGeneration>(this);
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
    string imageResRef(_gameId == GameID::TSL ? "load_default" : "load_chargen");
    withLoadingScreen(imageResRef, [this]() {
        if (!_charGen) {
            loadCharacterGeneration();
        }
        playMusic(getCharacterGenerationMusic());
        changeScreen(GameScreen::CharacterGeneration);
    });
}

string Game::getCharacterGenerationMusic() const {
    switch (_gameId) {
        case GameID::TSL:
            return "mus_main";
        default:
            return "mus_theme_rep";
    }
}

void Game::quit() {
    _quit = true;
}

void Game::openInGame() {
    changeScreen(GameScreen::InGame);
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
    changeScreen(GameScreen::InGameMenu);
}

void Game::startDialog(const shared_ptr<SpatialObject> &owner, const string &resRef) {
    shared_ptr<GffStruct> dlg(Resources::instance().getGFF(resRef, ResourceType::Dlg));
    if (!dlg) {
        warn("Game: conversation not found: " + resRef);
        return;
    }

    stopMovement();
    setCursorType(CursorType::Default);
    changeScreen(GameScreen::Conversation);

    auto dialog = make_shared<Dialog>(resRef);
    dialog->load(*dlg);

    bool computerConversation = dialog->conversationType() == ConversationType::Computer;
    _conversation = computerConversation ? _computer.get() : static_cast<Conversation *>(_dialog.get());
    _conversation->start(dialog, owner);
}

void Game::stopMovement() {
    getActiveCamera()->stopMovement();
    _module->player().stopMovement();
}

void Game::openContainer(const shared_ptr<SpatialObject> &container) {
    stopMovement();
    setCursorType(CursorType::Default);
    _container->open(container);
    changeScreen(GameScreen::Container);
}

void Game::openPartySelection(const PartySelection::Context &ctx) {
    stopMovement();
    setCursorType(CursorType::Default);
    _partySelect->prepare(ctx);
    changeScreen(GameScreen::PartySelection);
}

void Game::openSaveLoad(SaveLoad::Mode mode) {
    setCursorType(CursorType::Default);
    _saveLoad->setMode(mode);
    _saveLoad->refresh();
    changeScreen(GameScreen::SaveLoad);
}

void Game::openLevelUp() {
    setCursorType(CursorType::Default);
    _charGen->startLevelUp();
    changeScreen(GameScreen::CharacterGeneration);
}

void Game::scheduleModuleTransition(const string &moduleName, const string &entry) {
    _nextModule = moduleName;
    _nextEntry = entry;
}

void Game::updateCamera(float dt) {
    switch (_screen) {
        case GameScreen::Conversation: {
            int cameraId;
            CameraType cameraType = _conversation->getCamera(cameraId);
            if (cameraType == CameraType::Static) {
                _module->area()->setStaticCamera(cameraId);
            }
            _cameraType = cameraType;
            break;
        }
        case GameScreen::InGame:
            if (_cameraType != CameraType::FirstPerson && _cameraType != CameraType::ThirdPerson) {
                _cameraType = CameraType::ThirdPerson;
            }
            break;
        default:
            break;
    }
    Camera *camera = getActiveCamera();
    if (camera) {
        camera->update(dt);
        glm::vec3 position(camera->sceneNode()->absoluteTransform()[3]);
        AudioPlayer::instance().setListenerPosition(position);
    }
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
    if (event.button != SDL_BUTTON_LEFT) return false;

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
            if (_gameSpeed < 8.0f) {
                _gameSpeed = glm::min(8.0f, _gameSpeed + 1.0f);
            }
            return true;

        case SDLK_v:
            if (_screen == GameScreen::InGame) {
                toggleInGameCameraType();
            }
            return true;
    }

    return false;
}

bool Game::getGlobalBoolean(const string &name) const {
    auto maybeValue = _globalBooleans.find(name);
    return maybeValue != _globalBooleans.end() ? maybeValue->second : false;
}

int Game::getGlobalNumber(const string &name) const {
    auto maybeValue = _globalNumbers.find(name);
    return maybeValue != _globalNumbers.end() ? maybeValue->second : 0;
}

string Game::getGlobalString(const string &name) const {
    auto maybeValue = _globalStrings.find(name);
    return maybeValue != _globalStrings.end() ? maybeValue->second : "";
}

shared_ptr<Location> Game::getGlobalLocation(const string &name) const {
    auto maybeLocation = _globalLocations.find(name);
    return maybeLocation != _globalLocations.end() ? maybeLocation->second : nullptr;
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

void Game::setGlobalString(const string &name, const string &value) {
    _globalStrings[name] = value;
}

void Game::setGlobalLocation(const string &name, const shared_ptr<Location> &location) {
    _globalLocations[name] = location;
}

void Game::setLocalBoolean(uint32_t objectId, int index, bool value) {
    _localBooleans[objectId][index] = value;
}

void Game::setLocalNumber(uint32_t objectId, int index, int value) {
    _localNumbers[objectId][index] = value;
}

void Game::setRunScriptVar(int var) {
    _runScriptVar = var;
}

void Game::setPaused(bool paused) {
    _paused = paused;
}

} // namespace game

} // namespace reone
