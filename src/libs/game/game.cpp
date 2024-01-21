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
#include "reone/audio/mixer.h"
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
#include "reone/system/threadutil.h"

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
    auto console = std::make_unique<Console>(_options, _services.graphics, _services.resource);
    console->init();
    console->registerCommand("info", "information on selected object", std::bind(&Game::cmdInfo, this, std::placeholders::_1));
    console->registerCommand("listglobals", "list global variables", std::bind(&Game::cmdListGlobals, this, std::placeholders::_1));
    console->registerCommand("listlocals", "list local variables", std::bind(&Game::cmdListLocals, this, std::placeholders::_1));
    console->registerCommand("runscript", "run script", std::bind(&Game::cmdRunScript, this, std::placeholders::_1));
    console->registerCommand("listanim", "list animations of selected object", std::bind(&Game::cmdListAnim, this, std::placeholders::_1));
    console->registerCommand("playanim", "play animation on selected object", std::bind(&Game::cmdPlayAnim, this, std::placeholders::_1));
    console->registerCommand("warp", "warp to a module", std::bind(&Game::cmdWarp, this, std::placeholders::_1));
    console->registerCommand("kill", "kill selected object", std::bind(&Game::cmdKill, this, std::placeholders::_1));
    console->registerCommand("additem", "add item to selected object", std::bind(&Game::cmdAddItem, this, std::placeholders::_1));
    console->registerCommand("givexp", "give experience to selected creature", std::bind(&Game::cmdGiveXP, this, std::placeholders::_1));
    console->registerCommand("showaabb", "toggle rendering AABB", std::bind(&Game::cmdShowAABB, this, std::placeholders::_1));
    console->registerCommand("showwalkmesh", "toggle rendering walkmesh", std::bind(&Game::cmdShowWalkmesh, this, std::placeholders::_1));
    console->registerCommand("showtriggers", "toggle rendering triggers", std::bind(&Game::cmdShowTriggers, this, std::placeholders::_1));
    _console = std::move(console);

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
}

void Game::update(float frameTime) {
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
}

void Game::render() {
    if (_movie) {
        _movie->render();
    } else {
        renderScene();
        renderGUI();
    }
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
        firstPerson->setPosition(thirdPerson->sceneNode()->origin());
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
        globals.projectionInv = glm::inverse(globals.projection);
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
        return;
    }
    auto clip = _services.resource.audioClips.get(_musicResRef);
    _music = _services.audio.mixer.play(std::move(clip), AudioType::Music);
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
                listenerPosition = partyLeader->position() + glm::vec3 {0.0f, 0.0f, 1.7f}; // TODO: height based on appearance
            }
        } else {
            listenerPosition = camera->sceneNode()->origin();
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
    sceneGraph.setRenderAABB(isShowAABBEnabled());
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

void Game::cmdInfo(const Console::TokenList &tokens) {
    auto object = module()->area()->selectedObject();
    if (!object) {
        _console->printLine("No object is selected");
        return;
    }
    glm::vec3 position(object->position());

    std::stringstream ss;
    ss << std::setprecision(2) << std::fixed
       << "id=" << object->id()
       << " "
       << "tag=\"" << object->tag() << "\""
       << " "
       << "tpl=\"" << object->blueprintResRef() << "\""
       << " "
       << "pos=[" << position.x << ", " << position.y << ", " << position.z << "]";

    switch (object->type()) {
    case ObjectType::Creature: {
        auto creature = std::static_pointer_cast<Creature>(object);
        ss << " "
           << "app=" << creature->appearance()
           << " "
           << "fac=" << static_cast<int>(creature->faction());
        break;
    }
    case ObjectType::Placeable: {
        auto placeable = std::static_pointer_cast<Placeable>(object);
        ss << " "
           << "app=" << placeable->appearance();
        break;
    }
    default:
        break;
    }

    _console->printLine(ss.str());
}

void Game::cmdListGlobals(const Console::TokenList &tokens) {
    auto &strings = globalStrings();
    for (auto &var : strings) {
        _console->printLine(var.first + " = " + var.second);
    }

    auto &booleans = globalBooleans();
    for (auto &var : booleans) {
        _console->printLine(var.first + " = " + (var.second ? "true" : "false"));
    }

    auto &numbers = globalNumbers();
    for (auto &var : numbers) {
        _console->printLine(var.first + " = " + std::to_string(var.second));
    }

    auto &locations = globalLocations();
    for (auto &var : locations) {
        _console->printLine(str(boost::format("%s = (%.04f, %.04f, %.04f, %.04f") %
                                var.first %
                                var.second->position().x %
                                var.second->position().y %
                                var.second->position().z %
                                var.second->facing()));
    }
}

void Game::cmdListLocals(const Console::TokenList &tokens) {
    auto object = module()->area()->selectedObject();
    if (!object) {
        _console->printLine("No object is selected");
        return;
    }

    auto &booleans = object->localBooleans();
    for (auto &var : booleans) {
        _console->printLine(std::to_string(var.first) + " -> " + (var.second ? "true" : "false"));
    }

    auto &numbers = object->localNumbers();
    for (auto &var : numbers) {
        _console->printLine(std::to_string(var.first) + " -> " + std::to_string(var.second));
    }
}

void Game::cmdListAnim(const Console::TokenList &tokens) {
    auto object = module()->area()->selectedObject();
    if (!object) {
        object = party().getLeader();
        if (!object) {
            _console->printLine("No object is selected");
            return;
        }
    }

    std::string substr;
    if (static_cast<int>(tokens.size()) > 1) {
        substr = tokens[1];
    }

    auto model = std::static_pointer_cast<ModelSceneNode>(object->sceneNode());
    std::vector<std::string> anims(model->model().getAnimationNames());
    sort(anims.begin(), anims.end());

    for (auto &anim : anims) {
        if (substr.empty() || boost::contains(anim, substr)) {
            _console->printLine(anim);
        }
    }
}

void Game::cmdPlayAnim(const Console::TokenList &tokens) {
    if (tokens.size() < 2) {
        _console->printLine("Usage: playanim anim_name");
        return;
    }
    auto object = module()->area()->selectedObject();
    if (!object) {
        object = party().getLeader();
        if (!object) {
            _console->printLine("No object is selected");
            return;
        }
    }
    auto model = std::static_pointer_cast<ModelSceneNode>(object->sceneNode());
    model->playAnimation(tokens[1], nullptr, AnimationProperties::fromFlags(AnimationFlags::loop));
}

void Game::cmdKill(const Console::TokenList &tokens) {
    auto object = module()->area()->selectedObject();
    if (!object) {
        _console->printLine("No object is selected");
        return;
    }
    auto effect = newEffect<DamageEffect>(
        100000,
        DamageType::Universal,
        DamagePower::Normal,
        std::shared_ptr<Creature>());
    object->applyEffect(std::move(effect), DurationType::Instant);
}

void Game::cmdAddItem(const Console::TokenList &tokens) {
    if (tokens.size() < 2) {
        _console->printLine("Usage: additem item_tpl [size]");
        return;
    }
    auto object = module()->area()->selectedObject();
    if (!object) {
        object = party().getLeader();
        if (!object) {
            _console->printLine("No object is selected");
            return;
        }
    }
    int stackSize = static_cast<int>(tokens.size()) > 2 ? stoi(tokens[2]) : 1;
    object->addItem(tokens[1], stackSize);
}

void Game::cmdGiveXP(const Console::TokenList &tokens) {
    if (tokens.size() < 2) {
        _console->printLine("Usage: givexp amount");
        return;
    }

    auto object = module()->area()->selectedObject();
    if (!object) {
        object = party().getLeader();
    }
    if (!object || object->type() != ObjectType::Creature) {
        _console->printLine("No creature is selected");
        return;
    }

    int amount = stoi(tokens[1]);
    std::static_pointer_cast<Creature>(object)->giveXP(amount);
}

void Game::cmdWarp(const Console::TokenList &tokens) {
    if (tokens.size() < 2) {
        _console->printLine("Usage: warp module");
        return;
    }
    loadModule(tokens[1]);
}

void Game::cmdRunScript(const Console::TokenList &tokens) {
    if (tokens.size() < 3) {
        _console->printLine("Usage: runscript resref caller_id [triggerrer_id [event_number [script_var]]], e.g. runscript k_ai_master 1 2 3 4");
        return;
    }

    std::string resRef = tokens[1];
    auto callerId = static_cast<uint32_t>(stoi(tokens[2]));
    auto triggerrerId = tokens.size() > 3 ? static_cast<uint32_t>(stoi(tokens[3])) : kObjectInvalid;
    int eventNumber = tokens.size() > 4 ? stoi(tokens[4]) : -1;
    int scriptVar = tokens.size() > 5 ? stoi(tokens[5]) : -1;

    int result = scriptRunner().run(resRef, callerId, triggerrerId, eventNumber, scriptVar);
    _console->printLine(str(boost::format("%s -> %d") % resRef % result));
}

void Game::cmdShowAABB(const Console::TokenList &tokens) {
    if (tokens.size() < 2) {
        _console->printLine("Usage: showaabb 1|0");
        return;
    }
    bool show = stoi(tokens[1]);
    setShowAABB(show);
}

void Game::cmdShowWalkmesh(const Console::TokenList &tokens) {
    if (tokens.size() < 2) {
        _console->printLine("Usage: showwalkmesh 1|0");
        return;
    }
    bool show = stoi(tokens[1]);
    setShowWalkmesh(show);
}

void Game::cmdShowTriggers(const Console::TokenList &tokens) {
    if (tokens.size() < 2) {
        _console->printLine("Usage: showtriggers 1|0");
        return;
    }
    bool show = stoi(tokens[1]);
    setShowTriggers(show);
}

} // namespace game

} // namespace reone
