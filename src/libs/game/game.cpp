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

#include "reone/game/game.h"

#include "reone/audio/context.h"
#include "reone/audio/di/services.h"
#include "reone/audio/player.h"
#include "reone/game/combat.h"
#include "reone/game/debug.h"
#include "reone/game/di/services.h"
#include "reone/game/location.h"
#include "reone/game/party.h"
#include "reone/game/script/routines.h"
#include "reone/game/surfaces.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/format/tgawriter.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/renderbuffer.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/gui/gui.h"
#include "reone/movie/format/bikreader.h"
#include "reone/resource/2da.h"
#include "reone/resource/di/services.h"
#include "reone/resource/director.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/format/erfreader.h"
#include "reone/resource/format/erfwriter.h"
#include "reone/resource/format/gffwriter.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/audioclips.h"
#include "reone/resource/provider/cursors.h"
#include "reone/resource/provider/dialogs.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/lips.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/movies.h"
#include "reone/resource/provider/scripts.h"
#include "reone/resource/provider/soundsets.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/provider/walkmeshes.h"
#include "reone/resource/resources.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/scene/render/pipeline.h"
#include "reone/script/di/services.h"
#include "reone/system/binarywriter.h"
#include "reone/system/clock.h"
#include "reone/system/di/services.h"
#include "reone/system/exception/validation.h"
#include "reone/system/fileutil.h"
#include "reone/system/logutil.h"

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::movie;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

void Game::init() {
    initLocalServices();
    setSceneSurfaces();
    setCursorType(CursorType::Default);

    _moduleNames = _services.resource.director.moduleNames();

    playVideo("legal");
    openMainMenu();
}

void Game::initLocalServices() {
    auto console = std::make_unique<Console>(*this, _services);
    console->init();
    _console = std::move(console);

    auto profileOverlay = std::make_unique<ProfileOverlay>(_services, _options);
    profileOverlay->init();
    _profileOverlay = std::move(profileOverlay);

    auto routines = std::make_unique<Routines>(_gameId, this, &_services);
    routines->init();
    _routines = std::move(routines);

    _scriptRunner = std::make_unique<ScriptRunner>(*_routines, _services.resource.scripts);

    _map = std::make_unique<Map>(*this, _services);
}

void Game::setSceneSurfaces() {
    auto walkable = _services.game.surfaces.getWalkableSurfaces();
    auto walkcheck = _services.game.surfaces.getWalkcheckSurfaces();
    auto lineOfSight = _services.game.surfaces.getLineOfSightSurfaces();
    for (auto &name : _services.scene.graphs.sceneNames()) {
        auto &scene = _services.scene.graphs.get(name);
        scene.setWalkableSurfaces(walkable);
        scene.setWalkcheckSurfaces(walkcheck);
        scene.setLineOfSightSurfaces(lineOfSight);
    }
}

bool Game::handle(const input::Event &event) {
    return _profileOverlay->timeInput([this, &event]() {
        switch (event.type) {
        case input::EventType::KeyDown:
            if (handleKeyDown(event.key)) {
                return true;
            }
            break;
        case input::EventType::MouseMotion:
            if (handleMouseMotion(event.motion)) {
                return true;
            }
            break;
        case input::EventType::MouseButtonDown:
            if (handleMouseButtonDown(event.button)) {
                return true;
            }
            break;
        case input::EventType::MouseButtonUp:
            if (handleMouseButtonUp(event.button)) {
                return true;
            }
            break;
        default:
            break;
        }

        if (_profileOverlay->handle(event)) {
            return true;
        }

        if (!_movie) {
            auto gui = getScreenGUI();
            if (gui && gui->handle(event)) {
                return true;
            }
            switch (_screen) {
            case Screen::InGame: {
                if (_console->handle(event)) {
                    return true;
                }
                if (_party.handle(event)) {
                    return true;
                }
                auto camera = getActiveCamera();
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

        return false;
    });
}

void Game::update(float frameTime) {
    _profileOverlay->timeUpdate([this, &frameTime]() {
        float dt = frameTime * _gameSpeed;
        if (_movie) {
            updateMovie(dt);
            return;
        }
        updateMusic();

        if (!_nextModule.empty()) {
            loadNextModule();
        }
        updateCamera(dt);

        bool updModule = !_movie && _module && (_screen == Screen::InGame || _screen == Screen::Conversation);
        if (updModule && !_paused) {
            _module->update(dt);
            _combat.update(dt);
        }

        auto gui = getScreenGUI();
        if (gui) {
            gui->update(dt);
        }
        updateSceneGraph(dt);

        _profileOverlay->update(dt);
    });
}

void Game::render() {
    _profileOverlay->timeRender([this]() {
        if (_movie) {
            _movie->render();
        } else {
            renderScene();
            renderGUI();
        }
    });
    _profileOverlay->onFrameEnded();
}

bool Game::handleKeyDown(const input::KeyEvent &event) {
    if (event.repeat)
        return false;

    switch (event.code) {
    case input::KeyCode::Minus:
        if (_options.game.developer && _gameSpeed > 1.0f) {
            _gameSpeed = glm::max(1.0f, _gameSpeed - 1.0f);
            return true;
        }
        break;

    case input::KeyCode::Equals:
        if (_options.game.developer && _gameSpeed < 8.0f) {
            _gameSpeed = glm::min(8.0f, _gameSpeed + 1.0f);
            return true;
        }
        break;

    case input::KeyCode::V:
        if (_options.game.developer && _screen == Screen::InGame) {
            toggleInGameCameraType();
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

bool Game::handleMouseMotion(const input::MouseMotionEvent &event) {
    _cursor->setPosition({event.x, event.y});
    return false;
}

bool Game::handleMouseButtonDown(const input::MouseButtonEvent &event) {
    if (event.button != input::MouseButton::Left) {
        return false;
    }
    _cursor->setPressed(true);
    if (_movie) {
        _movie->finish();
        return true;
    }
    return false;
}

bool Game::handleMouseButtonUp(const input::MouseButtonEvent &event) {
    if (event.button != input::MouseButton::Left) {
        return false;
    }
    _cursor->setPressed(false);
    return false;
}

void Game::loadModule(const std::string &name, std::string entry) {
    info("Loading module '" + name + "'");

    withLoadingScreen("load_" + name, [this, &name, &entry]() {
        loadInGameMenus();

        try {
            if (_module) {
                _module->area()->runOnExitScript();
                _module->area()->unloadParty();
            }

            _services.resource.director.onModuleLoad(name);

            if (_loadScreen) {
                _loadScreen->setProgress(50);
            }
            render();

            _services.scene.graphs.get(kSceneMain).clear();

            auto maybeModule = _loadedModules.find(name);
            if (maybeModule != _loadedModules.end()) {
                _module = maybeModule->second;
            } else {
                _module = newModule();
                _objectById.insert(std::make_pair(_module->id(), _module));

                std::shared_ptr<Gff> ifo(_services.resource.gffs.get("module", ResType::Ifo));
                if (!ifo) {
                    throw ResourceNotFoundException("Module IFO not found");
                }

                _module->load(name, *ifo);
                _loadedModules.insert(std::make_pair(name, _module));
            }

            if (_party.isEmpty()) {
                loadDefaultParty();
            }

            _module->loadParty(entry);

            info("Module '" + name + "' loaded successfully");

            if (_loadScreen) {
                _loadScreen->setProgress(100);
            }
            render();

            std::string musicName(_module->area()->music());
            playMusic(musicName);

            //_ticks = _services.system.clock.ticks();
            openInGame();
        } catch (const std::exception &e) {
            error("Failed loading module '" + name + "': " + std::string(e.what()));
        }
    });
}

void Game::loadDefaultParty() {
    std::string member1, member2, member3;
    _party.defaultMembers(member1, member2, member3);

    if (!member1.empty()) {
        std::shared_ptr<Creature> player = newCreature();
        _objectById.insert(std::make_pair(player->id(), player));
        player->loadFromBlueprint(member1);
        player->setTag(kObjectTagPlayer);
        player->setImmortal(true);
        _party.addMember(kNpcPlayer, player);
        _party.setPlayer(player);
    }
    if (!member2.empty()) {
        std::shared_ptr<Creature> companion = newCreature();
        _objectById.insert(std::make_pair(companion->id(), companion));
        companion->loadFromBlueprint(member2);
        companion->setImmortal(true);
        companion->equip("g_w_dblsbr001");
        _party.addMember(0, companion);
    }
    if (!member3.empty()) {
        std::shared_ptr<Creature> companion = newCreature();
        _objectById.insert(std::make_pair(companion->id(), companion));
        companion->loadFromBlueprint(member3);
        companion->setImmortal(true);
        _party.addMember(1, companion);
    }
}

void Game::setCursorType(CursorType type) {
    if (_cursorType == type) {
        return;
    }
    if (type == CursorType::None) {
        _cursor.reset();
    } else {
        _cursor = _services.resource.cursors.get(type);
    }
    _cursorType = type;
}

void Game::playVideo(const std::string &name) {
    _movie = _services.resource.movies.get(name);
    if (!_movie) {
        return;
    }

    if (_music) {
        _music->stop();
        _music.reset();
    }
}

void Game::playMusic(const std::string &resRef) {
    if (_musicResRef == resRef) {
        return;
    }
    if (_music) {
        _music->stop();
        _music.reset();
    }
    _musicResRef = resRef;
}

void Game::renderScene() {
    if (!_module) {
        return;
    }
    auto &scene = _services.scene.graphs.get(kSceneMain);
    auto &output = scene.render({_options.graphics.width, _options.graphics.height});
    _services.graphics.uniforms.setLocals(std::bind(&LocalUniforms::reset, std::placeholders::_1));
    _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::ndcTexture));
    _services.graphics.context.bindTexture(output);
    _services.graphics.meshRegistry.get(MeshName::quadNDC).draw();
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
        std::shared_ptr<Area> area(_module->area());
        auto thirdPerson = area->getCamera<ThirdPersonCamera>(CameraType::ThirdPerson);
        auto firstPerson = area->getCamera<FirstPersonCamera>(CameraType::FirstPerson);
        firstPerson->setPosition(thirdPerson->sceneNode()->getOrigin());
        firstPerson->setFacing(thirdPerson->facing());
        _cameraType = CameraType::FirstPerson;
        break;
    }
    default:
        break;
    }

    setRelativeMouseMode(_cameraType == CameraType::FirstPerson);

    _module->area()->updateRoomVisibility();
}

Camera *Game::getActiveCamera() const {
    if (!_module) {
        return nullptr;
    }
    std::shared_ptr<Area> area(_module->area());
    if (!area) {
        return nullptr;
    }
    return area->getCamera(_cameraType);
}

std::shared_ptr<Object> Game::getObjectById(uint32_t id) const {
    switch (id) {
    case kObjectSelf:
        throw std::invalid_argument("Invalid id: " + std::to_string(id));
    case kObjectInvalid:
        return nullptr;
    default: {
        auto it = _objectById.find(id);
        return it != _objectById.end() ? it->second : nullptr;
    }
    }
}

void Game::renderGUI() {
    _services.graphics.uniforms.setGlobals([this](auto &globals) {
        globals.reset();
        globals.projection = glm::ortho(
            0.0f,
            static_cast<float>(_options.graphics.width),
            static_cast<float>(_options.graphics.height),
            0.0f, 0.0f, 100.0f);
    });
    switch (_screen) {
    case Screen::InGame:
        if (_cameraType == CameraType::ThirdPerson) {
            renderHUD();
        }
        if (_console->isOpen()) {
            _console->render();
        }
        break;

    default: {
        auto gui = getScreenGUI();
        if (gui) {
            gui->render();
        }
        break;
    }
    }
    _profileOverlay->render();
    if (_cursor && !_relativeMouseMode) {
        _cursor->render();
    }
}

void Game::updateMovie(float dt) {
    _movie->update(dt);

    if (_movie->isFinished()) {
        _movie.reset();
    }
}

void Game::updateMusic() {
    if (_musicResRef.empty()) {
        return;
    }
    if (_music && _music->isPlaying()) {
        _music->update();
    } else {
        _music = _services.audio.player.play(_services.resource.audioClips.get(_musicResRef), AudioType::Music);
    }
}

void Game::loadNextModule() {
    loadModule(_nextModule, _nextEntry);

    _nextModule.clear();
    _nextEntry.clear();
}

void Game::stopMovement() {
    auto camera = getActiveCamera();
    if (camera) {
        camera->stopMovement();
    }
    _module->player().stopMovement();
}

void Game::scheduleModuleTransition(const std::string &moduleName, const std::string &entry) {
    _nextModule = moduleName;
    _nextEntry = entry;
}

void Game::updateCamera(float dt) {
    switch (_screen) {
    case Screen::Conversation: {
        int cameraId;
        CameraType cameraType = getConversationCamera(cameraId);
        if (cameraType == CameraType::Static) {
            _module->area()->setStaticCamera(cameraId);
        }
        _cameraType = cameraType;
        break;
    }
    case Screen::InGame:
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
            std::shared_ptr<Creature> partyLeader(_party.getLeader());
            if (partyLeader) {
                listenerPosition = partyLeader->position() + 1.7f; // TODO: height based on appearance
            }
        } else {
            listenerPosition = camera->sceneNode()->getOrigin();
        }
        _services.audio.context.setListenerPosition(std::move(listenerPosition));
    }
}

void Game::updateSceneGraph(float dt) {
    auto camera = getActiveCamera();
    if (!camera) {
        return;
    }
    auto &sceneGraph = _services.scene.graphs.get(kSceneMain);
    sceneGraph.setActiveCamera(camera->cameraSceneNode().get());
    sceneGraph.setUpdateRoots(!_paused);
    sceneGraph.setRenderWalkmeshes(isShowWalkmeshEnabled());
    sceneGraph.setRenderTriggers(isShowTriggersEnabled());
    sceneGraph.update(dt);
}

bool Game::getGlobalBoolean(const std::string &name) const {
    auto it = _globalBooleans.find(name);
    return it != _globalBooleans.end() ? it->second : false;
}

int Game::getGlobalNumber(const std::string &name) const {
    auto it = _globalNumbers.find(name);
    return it != _globalNumbers.end() ? it->second : 0;
}

std::string Game::getGlobalString(const std::string &name) const {
    auto it = _globalStrings.find(name);
    return it != _globalStrings.end() ? it->second : "";
}

std::shared_ptr<Location> Game::getGlobalLocation(const std::string &name) const {
    auto it = _globalLocations.find(name);
    return it != _globalLocations.end() ? it->second : nullptr;
}

void Game::setGlobalBoolean(const std::string &name, bool value) {
    _globalBooleans[name] = value;
}

void Game::setGlobalNumber(const std::string &name, int value) {
    _globalNumbers[name] = value;
}

void Game::setGlobalString(const std::string &name, const std::string &value) {
    _globalStrings[name] = value;
}

void Game::setGlobalLocation(const std::string &name, const std::shared_ptr<Location> &location) {
    _globalLocations[name] = location;
}

void Game::setPaused(bool paused) {
    _paused = paused;
}

void Game::setRelativeMouseMode(bool relative) {
    _relativeMouseMode = relative;
}

void Game::withLoadingScreen(const std::string &imageResRef, const std::function<void()> &block) {
    if (!_loadScreen) {
        _loadScreen = tryLoadGUI<LoadingScreen>();
    }
    if (_loadScreen) {
        _loadScreen->setImage(imageResRef);
        _loadScreen->setProgress(0);
    }
    changeScreen(Screen::Loading);
    render();
    block();
}

void Game::openMainMenu() {
    if (!_mainMenu) {
        _mainMenu = tryLoadGUI<MainMenu>();
    }
    if (!_mainMenu) {
        return;
    }
    if (!_saveLoad) {
        _saveLoad = tryLoadGUI<SaveLoad>();
    }
    playMusic(_mainMenu->musicResRef());
    changeScreen(Screen::MainMenu);
}

void Game::openInGame() {
    changeScreen(Screen::InGame);
}

void Game::openInGameMenu(InGameMenuTab tab) {
    setCursorType(CursorType::Default);
    switch (tab) {
    case InGameMenuTab::Equipment:
        _inGame->openEquipment();
        break;
    case InGameMenuTab::Inventory:
        _inGame->openInventory();
        break;
    case InGameMenuTab::Character:
        _inGame->openCharacter();
        break;
    case InGameMenuTab::Abilities:
        _inGame->openAbilities();
        break;
    case InGameMenuTab::Messages:
        _inGame->openMessages();
        break;
    case InGameMenuTab::Journal:
        _inGame->openJournal();
        break;
    case InGameMenuTab::Map:
        _inGame->openMap();
        break;
    case InGameMenuTab::Options:
        _inGame->openOptions();
        break;
    default:
        break;
    }
    changeScreen(Screen::InGameMenu);
}

void Game::openContainer(const std::shared_ptr<Object> &container) {
    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _container->open(container);
    changeScreen(Screen::Container);
}

void Game::openPartySelection(const PartySelectionContext &ctx) {
    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _partySelect->prepare(ctx);
    changeScreen(Screen::PartySelection);
}

void Game::openSaveLoad(SaveLoadMode mode) {
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _saveLoad->setMode(mode);
    _saveLoad->refresh();
    changeScreen(Screen::SaveLoad);
}

void Game::openLevelUp() {
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _charGen->startLevelUp();
    changeScreen(Screen::CharacterGeneration);
}

void Game::startCharacterGeneration() {
    if (!_charGen) {
        _charGen = tryLoadGUI<CharacterGeneration>();
    }
    if (!_charGen) {
        return;
    }
    withLoadingScreen(_charGen->loadScreenResRef(), [this]() {
        _loadScreen->setProgress(100);
        render();
        playMusic(_charGen->musicResRef());
        changeScreen(Screen::CharacterGeneration);
    });
}

void Game::startDialog(const std::shared_ptr<Object> &owner, const std::string &resRef) {
    std::shared_ptr<Gff> dlg(_services.resource.gffs.get(resRef, ResType::Dlg));
    if (!dlg) {
        warn("Game: conversation not found: " + resRef);
        return;
    }

    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    changeScreen(Screen::Conversation);

    auto dialog = _services.resource.dialogs.get(resRef);
    bool computerConversation = dialog->conversationType == ConversationType::Computer;
    _conversation = computerConversation ? _computer.get() : static_cast<Conversation *>(_dialog.get());
    _conversation->start(dialog, owner);
}

void Game::resumeConversation() {
    _conversation->resume();
}

void Game::pauseConversation() {
    _conversation->pause();
}

void Game::loadInGameMenus() {
    if (!_hud) {
        _hud = tryLoadGUI<HUD>();
    }
    if (!_inGame) {
        _inGame = tryLoadGUI<InGameMenu>();
    }
    if (!_dialog) {
        _dialog = tryLoadGUI<DialogGUI>();
    }
    if (!_computer) {
        _computer = tryLoadGUI<ComputerGUI>();
    }
    if (!_container) {
        _container = tryLoadGUI<ContainerGUI>();
    }
    if (!_partySelect) {
        _partySelect = tryLoadGUI<PartySelection>();
    }
}

void Game::changeScreen(Screen screen) {
    auto gui = getScreenGUI();
    if (gui) {
        gui->clearSelection();
    }
    _screen = screen;
}

GameGUI *Game::getScreenGUI() const {
    switch (_screen) {
    case Screen::MainMenu:
        return _mainMenu.get();
    case Screen::Loading:
        return _loadScreen.get();
    case Screen::CharacterGeneration:
        return _charGen.get();
    case Screen::InGame:
        return _cameraType == game::CameraType::ThirdPerson ? _hud.get() : nullptr;
    case Screen::InGameMenu:
        return _inGame.get();
    case Screen::Conversation:
        return _conversation;
    case Screen::Container:
        return _container.get();
    case Screen::PartySelection:
        return _partySelect.get();
    case Screen::SaveLoad:
        return _saveLoad.get();
    default:
        return nullptr;
    }
}

void Game::setBarkBubbleText(std::string text, float duration) {
    _hud->barkBubble().setBarkText(text, duration);
}

void Game::onModuleSelected(const std::string &module) {
    _mainMenu->onModuleSelected(module);
}

void Game::renderHUD() {
    _hud->render();
}

CameraType Game::getConversationCamera(int &cameraId) const {
    return _conversation->getCamera(cameraId);
}

} // namespace game

} // namespace reone
