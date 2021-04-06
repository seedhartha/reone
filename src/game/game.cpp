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

#include <boost/algorithm/string.hpp>

#include "SDL2/SDL_timer.h"

#include "../audio/files.h"
#include "../audio/player.h"
#include "../audio/soundhandle.h"
#include "../common/log.h"
#include "../common/pathutil.h"
#include "../render/featureutil.h"
#include "../render/lip/lips.h"
#include "../render/materials.h"
#include "../render/meshes.h"
#include "../render/model/mdlreader.h"
#include "../render/model/models.h"
#include "../render/pbribl.h"
#include "../render/textures.h"
#include "../render/walkmesh/walkmeshes.h"
#include "../render/window.h"
#include "../resource/resources.h"
#include "../resource/strings.h"
#include "../script/scripts.h"
#include "../video/bikreader.h"
#include "../video/video.h"

#include "cursors.h"
#include "gameidutil.h"
#include "reputes.h"
#include "gui/sounds.h"
#include "script/routines.h"
#include "soundsets.h"
#include "surfaces.h"

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

static constexpr char kDataDirectoryName[] = "data";
static constexpr char kModulesDirectoryName[] = "modules";

static bool g_conversationsEnabled = true;

Game::Game(const fs::path &path, const Options &opts) :
    _path(path),
    _options(opts),
    _sceneGraph(opts.graphics),
    _worldPipeline(&_sceneGraph, opts.graphics),
    _console(this),
    _party(this),
    _scriptRunner(this),
    _profileOverlay(opts.graphics),
    _combat(this),
    _objectFactory(this, &_sceneGraph) {

    _gameId = determineGameID(path);
}

int Game::run() {
    init();
    openMainMenu();

    if (!_options.module.empty()) {
        _mainMenu->onModuleSelected(_options.module);
    } else {
        playVideo("legal");
    }
    RenderWindow::instance().show();

    runMainLoop();
    deinit();

    return 0;
}

void Game::init() {
    initResourceProviders();
    loadModuleNames();

    RenderWindow::instance().init(_options.graphics, this);
    Strings::instance().init(_path);
    Meshes::instance().init();
    Textures::instance().init();
    Materials::instance().init();
    PBRIBL::instance().init();
    Shaders::instance().init();
    AudioPlayer::instance().init(_options.audio);
    GUISounds::instance().init();
    Routines::instance().init(this);
    Reputes::instance().init();
    Surfaces::instance().init();
    Walkmeshes::instance().init(Surfaces::instance().getWalkableSurfaceIndices(), Surfaces::instance().getGrassSurfaceIndices());

    Cursors::instance().init(_gameId);
    setCursorType(CursorType::Default);

    _worldPipeline.init();
    _console.load();
    _profileOverlay.init();
}

void Game::initResourceProviders() {
    if (isTSL(_gameId)) {
        initResourceProvidersForTSL();
    } else {
        initResourceProvidersForKotOR();
    }
    Resources::instance().indexDirectory(getPathIgnoreCase(fs::current_path(), kDataDirectoryName));
}

void Game::loadModuleNames() {
    fs::path modules(getPathIgnoreCase(_path, kModulesDirectoryName));

    for (auto &entry : fs::directory_iterator(modules)) {
        string filename(boost::to_lower_copy(entry.path().filename().string()));
        if (boost::ends_with(filename, ".mod") || (boost::ends_with(filename, ".rim") && !boost::ends_with(filename, "_s.rim"))) {
            string moduleName(boost::to_lower_copy(filename.substr(0, filename.size() - 4)));
            _moduleNames.insert(move(moduleName));
        }
    }
}

void Game::setCursorType(CursorType type) {
    if (_cursorType != type) {
        if (type == CursorType::None) {
            RenderWindow::instance().setCursor(nullptr);
        } else {
            RenderWindow::instance().setCursor(Cursors::instance().get(type));
        }
        _cursorType = type;
    }
}

void Game::playVideo(const string &name) {
    fs::path path(getPathIgnoreCase(_path, "movies/" + name + ".bik"));

    BikReader bik(path);
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
    return isTSL(_gameId) ? "mus_sion" : "mus_theme_cult";
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
    info("Load module " + name);

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
        AudioFiles::instance().invalidate();
        Scripts::instance().invalidate();
        SoundSets::instance().invalidate();
        Lips::instance().invalidate();

        loadModuleResources(name);

        if (_module) {
            _module->area()->runOnExitScript();
            _module->area()->unloadParty();
        }

        _loadScreen->setProgress(50);
        drawAll();

        auto maybeModule = _loadedModules.find(name);
        if (maybeModule != _loadedModules.end()) {
            _module = maybeModule->second;
        } else {
            shared_ptr<GffStruct> ifo(Resources::instance().getGFF("module", ResourceType::Ifo));

            _module = _objectFactory.newModule();
            _module->load(name, *ifo);

            _loadedModules.insert(make_pair(name, _module));
        }

        _module->loadParty(entry);
        _module->area()->fill(_sceneGraph);

        _loadScreen->setProgress(100);
        drawAll();

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
    _loadScreen->setProgress(0);
    changeScreen(GameScreen::Loading);
    drawAll();
    block();
}

void Game::loadModuleResources(const string &moduleName) {
    Resources::instance().invalidateCache();
    Resources::instance().clearTransientProviders();

    fs::path modulesPath(getPathIgnoreCase(_path, kModulesDirectoryName));
    fs::path modPath(getPathIgnoreCase(modulesPath, moduleName + ".mod"));
    if (fs::exists(modPath)) {
        Resources::instance().indexErfFile(getPathIgnoreCase(modulesPath, moduleName + ".mod", false), true);
    } else {
        Resources::instance().indexRimFile(getPathIgnoreCase(modulesPath, moduleName + ".rim"), true);
        Resources::instance().indexRimFile(getPathIgnoreCase(modulesPath, moduleName + "_s.rim"), true);
    }

    fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
    Resources::instance().indexErfFile(getPathIgnoreCase(lipsPath, moduleName + "_loc.mod"), true);

    if (isTSL(_gameId)) {
        Resources::instance().indexErfFile(getPathIgnoreCase(modulesPath, moduleName + "_dlg.erf"), true);
    }
}

void Game::drawAll() {
    // Compute derived PBR IBL textures from queued environment maps
    PBRIBL::instance().refresh();

    RenderWindow::instance().clear();

    if (_video) {
        _video->draw();
    } else {
        drawWorld();
        drawGUI();
        RenderWindow::instance().drawCursor();
    }

    _profileOverlay.draw();

    RenderWindow::instance().swapBuffers();
}

void Game::drawWorld() {
    const Camera *camera = getActiveCamera();
    if (!camera) return;

    shared_ptr<SceneNode> shadowReference;

    shared_ptr<Creature> partyLeader(_party.getLeader());
    if (partyLeader && _cameraType == CameraType::ThirdPerson) {
        shadowReference = partyLeader->getModelSceneNode();
    } else {
        shadowReference = camera->sceneNode();
    }

    _sceneGraph.setActiveCamera(camera->sceneNode());
    _sceneGraph.setShadowReference(shadowReference);
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
            auto &thirdPerson = area->getCamera<ThirdPersonCamera>(CameraType::ThirdPerson);
            auto &firstPerson = area->getCamera<FirstPersonCamera>(CameraType::FirstPerson);
            firstPerson.setPosition(thirdPerson.sceneNode()->absoluteTransform()[3]);
            firstPerson.setFacing(thirdPerson.facing());
            _cameraType = CameraType::FirstPerson;
            break;
        }
        default:
            break;
    }

    setRelativeMouseMode(_cameraType == CameraType::FirstPerson);
}

Camera *Game::getActiveCamera() const {
    return _module ? &(_module->area()->getCamera(_cameraType)) : nullptr;
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
    switch (_screen) {
        case GameScreen::InGame:
            if (_cameraType == CameraType::ThirdPerson) {
                _hud->draw();
            }
            if (_console.isOpen()) {
                _console.draw();
            }
            break;

        default: {
            GUI *gui = getScreenGUI();
            if (gui) {
                gui->draw();
                gui->draw3D();
            }
            break;
        }
    }
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
            return _cameraType == CameraType::ThirdPerson ? _hud.get() : nullptr;
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
        RenderWindow::instance().processEvents(_quit);

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

        if (!_paused) {
            _combat.update(dt);
        }
    }

    GUI *gui = getScreenGUI();
    if (gui) {
        gui->update(dt);
    }

    if (!_paused) {
        _sceneGraph.update(dt);
    }

    _profileOverlay.update(dt);
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
    AudioPlayer::instance().deinit();
    RenderWindow::instance().deinit();
}

void Game::startCharacterGeneration() {
    string imageResRef(isTSL(_gameId) ? "load_default" : "load_chargen");
    withLoadingScreen(imageResRef, [this]() {
        if (!_charGen) {
            loadCharacterGeneration();
        }
        _loadScreen->setProgress(100);
        drawAll();
        playMusic(getCharacterGenerationMusic());
        changeScreen(GameScreen::CharacterGeneration);
    });
}

string Game::getCharacterGenerationMusic() const {
    return isTSL(_gameId) ? "mus_main" : "mus_theme_rep";
}

void Game::quit() {
    _quit = true;
}

void Game::openInGame() {
    _sceneGraph.setUpdate(!_paused);
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
    if (!g_conversationsEnabled) return;

    shared_ptr<GffStruct> dlg(Resources::instance().getGFF(resRef, ResourceType::Dlg));
    if (!dlg) {
        warn("Game: conversation not found: " + resRef);
        return;
    }

    stopMovement();
    setRelativeMouseMode(false);
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
    _sceneGraph.setUpdate(false);
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _container->open(container);
    changeScreen(GameScreen::Container);
}

void Game::openPartySelection(const PartySelection::Context &ctx) {
    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _partySelect->prepare(ctx);
    changeScreen(GameScreen::PartySelection);
}

void Game::openSaveLoad(SaveLoad::Mode mode) {
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _saveLoad->setMode(mode);
    _saveLoad->refresh();
    changeScreen(GameScreen::SaveLoad);
}

void Game::openLevelUp() {
    setRelativeMouseMode(false);
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

        glm::vec3 listenerPosition;
        if (_cameraType == CameraType::ThirdPerson) {
            listenerPosition = _party.getLeader()->position() + 1.7f; // TODO: height based on appearance
        } else {
            listenerPosition = camera->sceneNode()->absoluteTransform()[3];
        }
        AudioPlayer::instance().setListenerPosition(listenerPosition);
    }
}

bool Game::handle(const SDL_Event &event) {
    if (_profileOverlay.handle(event)) return true;

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
            if (_options.developer && _gameSpeed > 1.0f) {
                _gameSpeed = glm::max(1.0f, _gameSpeed - 1.0f);
                return true;
            }
            break;

        case SDLK_EQUALS:
            if (_options.developer && _gameSpeed < 8.0f) {
                _gameSpeed = glm::min(8.0f, _gameSpeed + 1.0f);
                return true;
            }
            break;

        case SDLK_v:
            if (_options.developer && _screen == GameScreen::InGame) {
                toggleInGameCameraType();
                return true;
            }
            break;

        case SDLK_F1:
            if (_options.developer) {
                setFeatureEnabled(Feature::PBR, !isFeatureEnabled(Feature::PBR));
                return true;
            }
            break;

        case SDLK_F2:
            if (_options.developer) {
                setFeatureEnabled(Feature::HDR, !isFeatureEnabled(Feature::HDR));
                return true;
            }
            break;

        case SDLK_F3:
            if (_options.developer) {
                setFeatureEnabled(Feature::DynamicRoomLighting, !isFeatureEnabled(Feature::DynamicRoomLighting));
                return true;
            }
            break;

        case SDLK_LEFTBRACKET:
            _sceneGraph.setExposure(glm::max(0.25f, _sceneGraph.exposure() - 0.25f));
            return true;

        case SDLK_RIGHTBRACKET:
            _sceneGraph.setExposure(glm::min(_sceneGraph.exposure() + 0.25f, 2.0f));
            return true;

        default:
            break;
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

void Game::setRelativeMouseMode(bool relative) {
    RenderWindow::instance().setRelativeMouseMode(relative);
}

} // namespace game

} // namespace reone
