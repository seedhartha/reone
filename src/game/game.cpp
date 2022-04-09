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

#include "../../audio/context.h"
#include "../../audio/files.h"
#include "../../audio/player.h"
#include "../../audio/services.h"
#include "../../common/collectionutil.h"
#include "../../common/exception/validation.h"
#include "../../common/logutil.h"
#include "../../common/pathutil.h"
#include "../../common/streamutil.h"
#include "../../common/streamwriter.h"
#include "../../graphics/context.h"
#include "../../graphics/format/tgawriter.h"
#include "../../graphics/lipanimations.h"
#include "../../graphics/meshes.h"
#include "../../graphics/models.h"
#include "../../graphics/pipeline.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/shaders.h"
#include "../../graphics/textures.h"
#include "../../graphics/uniforms.h"
#include "../../graphics/walkmeshes.h"
#include "../../graphics/window.h"
#include "../../gui/gui.h"
#include "../../movie/format/bikreader.h"
#include "../../resource/2da.h"
#include "../../resource/2das.h"
#include "../../resource/format/erfreader.h"
#include "../../resource/format/erfwriter.h"
#include "../../resource/format/gffwriter.h"
#include "../../resource/gffs.h"
#include "../../resource/resources.h"
#include "../../resource/services.h"
#include "../../scene/graphs.h"
#include "../../script/scripts.h"

#include "combat.h"
#include "cursors.h"
#include "debug.h"
#include "location.h"
#include "object/factory.h"
#include "party.h"
#include "services.h"
#include "soundsets.h"
#include "surfaces.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;
using namespace reone::movie;

namespace fs = boost::filesystem;
namespace io = boost::iostreams;

namespace reone {

namespace game {

void Game::init() {
    // Surfaces
    auto walkableSurfaces = _services.surfaces.getWalkableSurfaces();
    auto walkcheckSurfaces = _services.surfaces.getWalkcheckSurfaces();
    auto lineOfSightSurfaces = _services.surfaces.getLineOfSightSurfaces();
    for (auto &scene : _services.sceneGraphs.scenes()) {
        scene.second->setWalkableSurfaces(walkableSurfaces);
        scene.second->setWalkcheckSurfaces(walkcheckSurfaces);
        scene.second->setLineOfSightSurfaces(lineOfSightSurfaces);
    }

    loadModuleNames();
    setCursorType(CursorType::Default);
}

int Game::run() {
    start();
    runMainLoop();
    return 0;
}

void Game::runMainLoop() {
    _ticks = SDL_GetTicks();
    while (!_quit) {
        _services.window.processEvents(_quit);
        if (!_services.window.isInFocus()) {
            continue;
        }
        update();
        drawAll();
    }
}

void Game::update() {
    float dt = measureFrameTime();

    if (_movie) {
        updateMovie(dt);
    } else {
        updateMusic();
    }
    if (!_nextModule.empty()) {
        loadNextModule();
    }
    updateCamera(dt);

    bool updModule = !_movie && _module && (_screen == GameScreen::InGame || _screen == GameScreen::Conversation);
    if (updModule && !_paused) {
        _module->update(dt);
        _combat.update(dt);
    }

    GUI *gui = getScreenGUI();
    if (gui) {
        gui->update(dt);
    }
    updateSceneGraph(dt);

    _profileOverlay->update(dt);
}

void Game::drawAll() {
    _services.graphicsContext.clearColorDepth();

    if (_movie) {
        _movie->draw();
    } else {
        drawWorld();
        drawGUI();
        _services.window.drawCursor();
    }

    _profileOverlay->draw();
    _services.window.swapBuffers();
}

void Game::loadModule(const string &name, string entry) {
    info("Load module '" + name + "'");

    withLoadingScreen("load_" + name, [this, &name, &entry]() {
        loadInGameMenus();

        _services.soundSets.invalidate();
        _services.textures.invalidate();
        _services.models.invalidate();
        _services.walkmeshes.invalidate();
        _services.lipAnimations.invalidate();
        _services.audio.files.invalidate();
        _services.scripts.invalidate();

        try {
            if (_module) {
                _module->area()->runOnExitScript();
                _module->area()->unloadParty();
            }

            loadModuleResources(name);
            if (_loadScreen) {
                _loadScreen->setProgress(50);
            }
            drawAll();

            _services.sceneGraphs.get(kSceneMain).clear();

            auto maybeModule = _loadedModules.find(name);
            if (maybeModule != _loadedModules.end()) {
                _module = maybeModule->second;
            } else {
                _module = _objectFactory.newModule();

                shared_ptr<GffStruct> ifo(_services.resource.gffs.get("module", ResourceType::Ifo));
                if (!ifo) {
                    throw ValidationException("Module IFO file not found");
                }

                _module->load(name, *ifo);
                _loadedModules.insert(make_pair(name, _module));
            }

            if (_party.isEmpty()) {
                loadDefaultParty();
            }

            _module->loadParty(entry);

            info("Module '" + name + "' loaded successfully");

            if (_loadScreen) {
                _loadScreen->setProgress(100);
            }
            drawAll();

            string musicName(_module->area()->music());
            playMusic(musicName);

            _ticks = SDL_GetTicks();
            openInGame();
        } catch (const ValidationException &e) {
            error("Failed loading module '" + name + "': " + string(e.what()));
        }
    });
}

void Game::loadDefaultParty() {
    string member1, member2, member3;
    getDefaultPartyMembers(member1, member2, member3);

    if (!member1.empty()) {
        shared_ptr<Creature> player(_objectFactory.newCreature());
        player->loadFromBlueprint(member1);
        player->setTag(kObjectTagPlayer);
        player->setImmortal(true);
        _party.addMember(kNpcPlayer, player);
        _party.setPlayer(player);
    }
    if (!member2.empty()) {
        shared_ptr<Creature> companion(_objectFactory.newCreature());
        companion->loadFromBlueprint(member2);
        companion->setImmortal(true);
        _party.addMember(0, companion);
    }
    if (!member3.empty()) {
        shared_ptr<Creature> companion(_objectFactory.newCreature());
        companion->loadFromBlueprint(member3);
        companion->setImmortal(true);
        _party.addMember(1, companion);
    }
}

void Game::setCursorType(CursorType type) {
    if (_cursorType != type) {
        if (type == CursorType::None) {
            _services.window.setCursor(nullptr);
        } else {
            _services.window.setCursor(_services.cursors.get(type));
        }
        _cursorType = type;
    }
}

void Game::playVideo(const string &name) {
    fs::path path(getPathIgnoreCase(_path, "movies/" + name + ".bik"));
    if (path.empty()) {
        return;
    }

    BikReader bik(
        path,
        _services.graphicsContext,
        _services.meshes,
        _services.shaders,
        _services.textures,
        _services.uniforms,
        _services.audio.player);
    bik.load();

    _movie = bik.movie();
    if (!_movie) {
        return;
    }

    if (_music) {
        _music->stop();
        _music.reset();
    }
}

void Game::playMusic(const string &resRef) {
    if (_musicResRef == resRef) {
        return;
    }
    if (_music) {
        _music->stop();
        _music.reset();
    }
    _musicResRef = resRef;
}

void Game::drawWorld() {
    auto &scene = _services.sceneGraphs.get(kSceneMain);
    auto output = _services.pipeline.draw(scene, glm::ivec2(_options.graphics.width, _options.graphics.height));
    if (!output) {
        return;
    }
    _services.uniforms.setGeneral([](auto &general) {
        general.resetGlobals();
        general.resetLocals();
    });
    _services.shaders.use(_services.shaders.simpleTexture());
    _services.textures.bind(*output);
    _services.meshes.quadNDC().draw();
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
        firstPerson.setPosition(thirdPerson.sceneNode()->getOrigin());
        firstPerson.setFacing(thirdPerson.facing());
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
    shared_ptr<Area> area(_module->area());
    if (!area) {
        return nullptr;
    }
    return &area->getCamera(_cameraType);
}

shared_ptr<Object> Game::getObjectById(uint32_t id) const {
    switch (id) {
    case kObjectSelf:
        throw invalid_argument("id is invalid");
    case kObjectInvalid:
        return nullptr;
    default:
        return _objectFactory.getObjectById(id);
    }
}

void Game::drawGUI() {
    switch (_screen) {
    case GameScreen::InGame:
        if (_cameraType == CameraType::ThirdPerson) {
            drawHUD();
        }
        if (_console->isOpen()) {
            _console->draw();
        }
        break;

    default: {
        GUI *gui = getScreenGUI();
        if (gui) {
            gui->draw();
        }
        break;
    }
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
        _music = _services.audio.player.play(_musicResRef, AudioType::Music);
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

void Game::quit() {
    _quit = true;
}

void Game::stopMovement() {
    getActiveCamera()->stopMovement();
    _module->player().stopMovement();
}

void Game::scheduleModuleTransition(const string &moduleName, const string &entry) {
    _nextModule = moduleName;
    _nextEntry = entry;
}

void Game::updateCamera(float dt) {
    switch (_screen) {
    case GameScreen::Conversation: {
        int cameraId;
        CameraType cameraType = getConversationCamera(cameraId);
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
            shared_ptr<Creature> partyLeader(_party.getLeader());
            if (partyLeader) {
                listenerPosition = partyLeader->position() + 1.7f; // TODO: height based on appearance
            }
        } else {
            listenerPosition = camera->sceneNode()->getOrigin();
        }
        _services.audio.context.setListenerPosition(move(listenerPosition));
    }
}

void Game::updateSceneGraph(float dt) {
    const Camera *camera = getActiveCamera();
    if (!camera) {
        return;
    }
    auto &sceneGraph = _services.sceneGraphs.get(kSceneMain);
    sceneGraph.setActiveCamera(camera->sceneNode());
    sceneGraph.setUpdateRoots(!_paused);
    sceneGraph.setDrawWalkmeshes(isShowWalkmeshEnabled());
    sceneGraph.setDrawTriggers(isShowTriggersEnabled());
    sceneGraph.update(dt);
}

bool Game::handle(const SDL_Event &event) {
    if (_profileOverlay->handle(event))
        return true;

    if (!_movie) {
        GUI *gui = getScreenGUI();
        if (gui && gui->handle(event)) {
            return true;
        }
        switch (_screen) {
        case GameScreen::InGame: {
            if (_console->handle(event)) {
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
        if (handleMouseButtonDown(event.button))
            return true;
        break;
    case SDL_KEYDOWN:
        if (handleKeyDown(event.key))
            return true;
        break;
    default:
        break;
    }

    return false;
}

bool Game::handleMouseButtonDown(const SDL_MouseButtonEvent &event) {
    if (event.button != SDL_BUTTON_LEFT)
        return false;

    if (_movie) {
        _movie->finish();
        return true;
    }

    return false;
}

bool Game::handleKeyDown(const SDL_KeyboardEvent &event) {
    if (event.repeat)
        return false;

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

    default:
        break;
    }

    return false;
}

bool Game::getGlobalBoolean(const string &name) const {
    return getFromLookupOrElse(_globalBooleans, name, false);
}

int Game::getGlobalNumber(const string &name) const {
    return getFromLookupOrElse(_globalNumbers, name, 0);
}

string Game::getGlobalString(const string &name) const {
    static string empty;
    return getFromLookupOrElse(_globalStrings, name, empty);
}

shared_ptr<Location> Game::getGlobalLocation(const string &name) const {
    return getFromLookupOrNull(_globalLocations, name);
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

void Game::setPaused(bool paused) {
    _paused = paused;
}

void Game::setRelativeMouseMode(bool relative) {
    _services.window.setRelativeMouseMode(relative);
}

void Game::withLoadingScreen(const string &imageResRef, const function<void()> &block) {
    if (!_loadScreen) {
        loadLoadingScreen();
    }
    if (_loadScreen) {
        _loadScreen->setImage(imageResRef);
        _loadScreen->setProgress(0);
    }
    changeScreen(GameScreen::Loading);
    drawAll();
    block();
}

} // namespace game

} // namespace reone
