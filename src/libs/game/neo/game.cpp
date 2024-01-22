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

#include "reone/game/neo/game.h"

#include "reone/game/console.h"
#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/camera.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/encounter.h"
#include "reone/game/neo/object/item.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/game/neo/object/sound.h"
#include "reone/game/neo/object/store.h"
#include "reone/game/neo/object/trigger.h"
#include "reone/game/neo/object/waypoint.h"
#include "reone/game/profiler.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/resource/di/services.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/layouts.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/paths.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/provider/visibilities.h"
#include "reone/resource/template/generated/are.h"
#include "reone/resource/template/generated/git.h"
#include "reone/resource/template/generated/ifo.h"
#include "reone/resource/template/generated/utc.h"
#include "reone/resource/template/generated/utd.h"
#include "reone/resource/template/generated/ute.h"
#include "reone/resource/template/generated/utm.h"
#include "reone/resource/template/generated/utp.h"
#include "reone/resource/template/generated/uts.h"
#include "reone/resource/template/generated/utt.h"
#include "reone/resource/template/generated/utw.h"
#include "reone/scene/collision.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/system/checkutil.h"
#include "reone/system/clock.h"
#include "reone/system/di/services.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/exception/validation.h"
#include "reone/system/threadutil.h"

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::resource::generated;
using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

static constexpr float kCameraDistance = 3.2f;
static constexpr float kCameraPitch = 84.0f;
static constexpr float kCameraHeight = 1.6f + 0.4f;

static constexpr float kCameraMouseSensitity = 0.001f;
static constexpr float kCameraMoveRate = 8.0f;
static constexpr float kCameraTurnRate = 4.0f;
static constexpr float kPlayerMoveRate = 4.0f;

static const std::string kPickedModelFormat {R"END(
Picked model:
  name: '%s'
)END"};

static const std::string kPickedObjectFormat {R"END(
Picked object:
  id: %d
  type: %d
  tag: '%s'
)END"};

static const std::string kLogicThreadName {"game"};

bool PlayerCameraController::handle(const input::Event &event) {
    if (_player) {
        switch (event.type) {
        case input::EventType::KeyDown:
            switch (event.key.code) {
            case input::KeyCode::W:
                _commandMask |= CommandTypes::MovePlayerFront;
                return true;
            case input::KeyCode::A:
                _commandMask |= CommandTypes::RotateCameraCCW;
                return true;
            case input::KeyCode::S:
                _commandMask |= CommandTypes::MovePlayerBack;
                return true;
            case input::KeyCode::D:
                _commandMask |= CommandTypes::RotateCameraCW;
                return true;
            case input::KeyCode::Z:
                _commandMask |= CommandTypes::MovePlayerLeft;
                return true;
            case input::KeyCode::C:
                _commandMask |= CommandTypes::MovePlayerRight;
                return true;
            default:
                break;
            }
            break;
        case input::EventType::KeyUp:
            switch (event.key.code) {
            case input::KeyCode::W:
                _commandMask &= ~CommandTypes::MovePlayerFront;
                return true;
            case input::KeyCode::A:
                _commandMask &= ~CommandTypes::RotateCameraCCW;
                return true;
            case input::KeyCode::S:
                _commandMask &= ~CommandTypes::MovePlayerBack;
                return true;
            case input::KeyCode::D:
                _commandMask &= ~CommandTypes::RotateCameraCW;
                return true;
            case input::KeyCode::Z:
                _commandMask &= ~CommandTypes::MovePlayerLeft;
                return true;
            case input::KeyCode::C:
                _commandMask &= ~CommandTypes::MovePlayerRight;
                return true;
            default:
                break;
            }
            break;
        default:
            break;
        }
    } else {
        switch (event.type) {
        case input::EventType::MouseMotion:
            _cameraPitch += -kCameraMouseSensitity * event.motion.yrel;
            _cameraFacing += -kCameraMouseSensitity * event.motion.xrel;
            refreshCamera();
            return true;
        case input::EventType::KeyDown:
            switch (event.key.code) {
            case input::KeyCode::W:
                _commandMask |= CommandTypes::MoveCameraFront;
                return true;
            case input::KeyCode::A:
                _commandMask |= CommandTypes::MoveCameraLeft;
                return true;
            case input::KeyCode::S:
                _commandMask |= CommandTypes::MoveCameraBack;
                return true;
            case input::KeyCode::D:
                _commandMask |= CommandTypes::MoveCameraRight;
                return true;
            case input::KeyCode::Q:
                _commandMask |= CommandTypes::MoveCameraUp;
                return true;
            case input::KeyCode::Z:
                _commandMask |= CommandTypes::MoveCameraDown;
                return true;
            default:
                break;
            }
            break;
        case input::EventType::KeyUp:
            switch (event.key.code) {
            case input::KeyCode::W:
                _commandMask &= ~CommandTypes::MoveCameraFront;
                return true;
            case input::KeyCode::A:
                _commandMask &= ~CommandTypes::MoveCameraLeft;
                return true;
            case input::KeyCode::S:
                _commandMask &= ~CommandTypes::MoveCameraBack;
                return true;
            case input::KeyCode::D:
                _commandMask &= ~CommandTypes::MoveCameraRight;
                return true;
            case input::KeyCode::Q:
                _commandMask &= ~CommandTypes::MoveCameraUp;
                return true;
            case input::KeyCode::Z:
                _commandMask &= ~CommandTypes::MoveCameraDown;
                return true;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    return false;
}

void PlayerCameraController::update(float dt) {
    if (!_commandMask) {
        return;
    }
    if (_player) {
        if (_commandMask & CommandTypes::RotateCameraCCW) {
            _cameraFacing += kCameraTurnRate * dt;
        } else if (_commandMask & CommandTypes::RotateCameraCW) {
            _cameraFacing -= kCameraTurnRate * dt;
        }
        float sinFacing = glm::sin(_cameraFacing);
        float cosFacing = glm::cos(_cameraFacing);
        float sinPitch = glm::sin(_cameraPitch - glm::half_pi<float>());
        float cosPitch = glm::cos(_cameraPitch - glm::half_pi<float>());
        auto &player = _player->get();
        _playerPosition = player.position();
        _playerFacing = player.facing();
        if (_commandMask & CommandTypes::MovePlayerFront) {
            _playerPosition.x -= kPlayerMoveRate * sinFacing * dt;
            _playerPosition.y += kPlayerMoveRate * cosFacing * dt;
            _playerPosition.z += 0.0f;
            _playerFacing = _cameraFacing;
        } else if (_commandMask & CommandTypes::MovePlayerBack) {
            _playerPosition.x += kPlayerMoveRate * sinFacing * dt;
            _playerPosition.y -= kPlayerMoveRate * cosFacing * dt;
            _playerPosition.z += 0.0f;
            _playerFacing = _cameraFacing + glm::pi<float>();
        } else if (_commandMask & CommandTypes::MovePlayerLeft) {
            _playerPosition.x -= kPlayerMoveRate * cosFacing * dt;
            _playerPosition.y -= kPlayerMoveRate * sinFacing * dt;
            _playerPosition.z += 0.0f;
            _playerFacing = _cameraFacing + glm::half_pi<float>();
        } else if (_commandMask & CommandTypes::MovePlayerRight) {
            _playerPosition.x += kPlayerMoveRate * cosFacing * dt;
            _playerPosition.y += kPlayerMoveRate * sinFacing * dt;
            _playerPosition.z += 0.0f;
            _playerFacing = _cameraFacing - glm::half_pi<float>();
        }
        _cameraPosition.x = _playerPosition.x + kCameraDistance * sinFacing * cosPitch;
        _cameraPosition.y = _playerPosition.y - kCameraDistance * cosFacing * cosPitch;
        _cameraPosition.z = _playerPosition.z + kCameraHeight - kCameraDistance * sinPitch;
        refreshCamera();
        refreshPlayer();
    } else {
        float sinFacing = glm::sin(_cameraFacing);
        float cosFacing = glm::cos(_cameraFacing);
        float sinPitch = glm::sin(_cameraPitch - glm::half_pi<float>());
        float cosPitch = glm::cos(_cameraPitch - glm::half_pi<float>());
        if (_commandMask & CommandTypes::MoveCameraRight) {
            _cameraPosition.x += kCameraMoveRate * cosFacing * dt;
            _cameraPosition.y += kCameraMoveRate * sinFacing * dt;
            _cameraPosition.z += 0.0f;
        } else if (_commandMask & CommandTypes::MoveCameraLeft) {
            _cameraPosition.x -= kCameraMoveRate * cosFacing * dt;
            _cameraPosition.y -= kCameraMoveRate * sinFacing * dt;
            _cameraPosition.z += 0.0f;
        } else if (_commandMask & CommandTypes::MoveCameraFront) {
            _cameraPosition.x -= kCameraMoveRate * sinFacing * dt;
            _cameraPosition.y += kCameraMoveRate * cosFacing * dt;
            _cameraPosition.z += kCameraMoveRate * sinPitch * dt;
        } else if (_commandMask & CommandTypes::MoveCameraBack) {
            _cameraPosition.x += kCameraMoveRate * sinFacing * dt;
            _cameraPosition.y -= kCameraMoveRate * cosFacing * dt;
            _cameraPosition.z -= kCameraMoveRate * sinPitch * dt;
        } else if (_commandMask & CommandTypes::MoveCameraUp) {
            _cameraPosition.x += 0.0f;
            _cameraPosition.y += 0.0f;
            _cameraPosition.z += kCameraMoveRate * dt;
        } else if (_commandMask & CommandTypes::MoveCameraDown) {
            _cameraPosition.x += 0.0f;
            _cameraPosition.y += 0.0f;
            _cameraPosition.z -= kCameraMoveRate * dt;
        }
        refreshCamera();
    }
}

void PlayerCameraController::refreshCamera() {
    auto &camera = _camera->get();
    auto transform = glm::translate(_cameraPosition);
    transform *= glm::eulerAngleZX(_cameraFacing, _cameraPitch);
    camera.setLocalTransform(std::move(transform));
}

void PlayerCameraController::refreshPlayer() {
    (*_gameLogicExecutor)([this]() {
        auto &player = _player->get();
        player.setPosition(_playerPosition);
        player.setFacing(_playerFacing);
    });
}

void Game::init() {
    checkThat(!_inited, "Must not be initialized");

    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    scene.setUpdateRoots(true);

    startModule("end_m01aa");

    if (_module) {
        auto &module = _module->get();
        auto &pc = loadCreature("", 23);
        _pc = pc;
        pc.setPosition(module.entryPosition());
        pc.setFacing(module.entryFacing());
        module.area().add(pc);
        _playerCameraController.setPlayer(pc);
        _playerCameraController.setGameLogicExecutor(std::bind(&Game::runOnLogicThread, this, std::placeholders::_1));

        auto camera = scene.newCamera();
        float aspect = _options.graphics.width / static_cast<float>(_options.graphics.height);
        camera->setPerspectiveProjection(glm::radians(55.0f), aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);
        _playerCameraController.setCamera(*camera);
        _playerCameraController.setCameraPosition(module.entryPosition());
        _playerCameraController.setCameraFacing(module.entryFacing());
        _playerCameraController.setCameraPitch(glm::radians(kCameraPitch));
        _playerCameraController.refreshCamera();
        _cameraSceneNode = *camera;
        scene.setActiveCamera(camera.get());
    }

    _profiler.reserveThread(kLogicThreadName);
    _logicThread = std::thread {std::bind(&Game::logicThreadFunc, this)};
    _inited = true;
}

void Game::deinit() {
    if (!_inited) {
        return;
    }
    if (_logicThread.joinable()) {
        _logicThread.join();
    }
    _inited = false;
}

bool Game::handle(const input::Event &event) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    if (_playerCameraController.handle(event)) {
        return true;
    }
    switch (event.type) {
    case input::EventType::MouseButtonDown:
        if (event.button.button != input::MouseButton::Left) {
            break;
        }
        if (_pickedModel) {
            std::string message;
            void *externalRef = _pickedModel->get().externalRef();
            if (!externalRef) {
                message = str(boost::format(kPickedModelFormat) % _pickedModel->get().model().name());
            } else {
                auto &object = *reinterpret_cast<SpatialObject *>(externalRef);
                message = str(boost::format(kPickedObjectFormat) %
                              object.id() %
                              static_cast<int>(object.type()) %
                              object.tag());
                runOnLogicThread([&object]() {
                    object.setFacing(object.facing() + glm::half_pi<float>());
                });
            }
            _console.printLine(message);
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

void Game::update(float dt) {
    handleEvents();
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    scene.update(dt);
    _playerCameraController.update(dt);
    if (_cameraSceneNode) {
        auto &camera = *_cameraSceneNode->get().camera();
        _pickedModel = scene.pickModelRay(camera.position(), camera.forward());
    }
}

void Game::handleEvents() {
    std::lock_guard<std::mutex> lock {_eventsMutex};
    for (auto &event : _events) {
        if (event.type == EventType::ObjectStateChanged && event.object.state == ObjectState::Loaded) {
            auto &object = static_cast<Object &>(_idToObject.at(event.object.objectId).get());
            if (object.type() == ObjectType::Area) {
                auto &area = static_cast<Area &>(object);
                onAreaLoaded(area);
            } else if (object.type() == ObjectType::Creature) {
                auto &creature = static_cast<Creature &>(object);
                onCreatureLoaded(creature);
            } else if (object.type() == ObjectType::Door) {
                auto &door = static_cast<Door &>(object);
                onDoorLoaded(door);
            } else if (object.type() == ObjectType::Placeable) {
                auto &placeable = static_cast<Placeable &>(object);
                onPlaceableLoaded(placeable);
            }
        } else if (event.type == EventType::ObjectLocationChanged) {
            auto &object = static_cast<SpatialObject &>(_idToObject.at(event.object.objectId).get());
            onObjectLocationChanged(object);
        }
    }
    _events.clear();
}

void Game::onAreaLoaded(Area &area) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    for (auto &room : area.rooms()) {
        auto model = _resourceSvc.models.get(room.model);
        if (!model) {
            throw ResourceNotFoundException("Room model not found: " + room.model);
        }
        auto sceneNode = scene.newModel(*model, ModelUsage::Room);
        sceneNode->setLocalTransform(glm::translate(room.position));
        scene.addRoot(std::move(sceneNode));
    }
}

void Game::onCreatureLoaded(Creature &creature) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    const auto &appearance = creature.appearance();
    auto modelName = appearance.model.value();
    auto model = _resourceSvc.models.get(modelName);
    if (!model) {
        throw ResourceNotFoundException("Creature model not found: " + modelName);
    }
    auto transform = glm::translate(creature.position());
    transform *= glm::eulerAngleZ(creature.facing());
    auto sceneNode = scene.newModel(*model, ModelUsage::Creature);
    if (appearance.texture) {
        auto &texName = appearance.texture->value();
        auto texture = _resourceSvc.textures.get(texName, TextureUsage::MainTex);
        if (!texture) {
            throw ResourceNotFoundException("Creature texture not found: " + texName);
        }
        sceneNode->setMainTexture(texture.get());
    }
    if (appearance.normalHeadModel) {
        auto headModel = _resourceSvc.models.get(appearance.normalHeadModel->value());
        if (!headModel) {
            throw ResourceNotFoundException("Creature head model not found: " + modelName);
        }
        auto headSceneNode = scene.newModel(*headModel, ModelUsage::Creature);
        sceneNode->attach("headhook", *headSceneNode);
    }
    sceneNode->setLocalTransform(std::move(transform));
    sceneNode->setDrawDistance(_options.graphics.drawDistance);
    sceneNode->setPickable(true);
    sceneNode->setExternalRef(&creature);
    scene.addRoot(std::move(sceneNode));
}

void Game::onDoorLoaded(Door &door) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    auto modelName = door.modelName().value();
    auto model = _resourceSvc.models.get(modelName);
    if (!model) {
        throw ResourceNotFoundException("Door model not found: " + modelName);
    }
    auto transform = glm::translate(door.position());
    transform *= glm::eulerAngleZ(door.facing());
    auto sceneNode = scene.newModel(*model, ModelUsage::Door);
    sceneNode->setLocalTransform(std::move(transform));
    // sceneNode->setDrawDistance(_options.graphics.drawDistance);
    sceneNode->setPickable(true);
    sceneNode->setExternalRef(&door);
    scene.addRoot(std::move(sceneNode));
}

void Game::onPlaceableLoaded(Placeable &placeable) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    auto modelName = placeable.modelName().value();
    auto model = _resourceSvc.models.get(modelName);
    if (!model) {
        throw ResourceNotFoundException("Placeable model not found: " + modelName);
    }
    auto transform = glm::translate(placeable.position());
    transform *= glm::eulerAngleZ(placeable.facing());
    auto sceneNode = scene.newModel(*model, ModelUsage::Placeable);
    sceneNode->setLocalTransform(std::move(transform));
    sceneNode->setDrawDistance(_options.graphics.drawDistance);
    sceneNode->setPickable(true);
    sceneNode->setExternalRef(&placeable);
    scene.addRoot(std::move(sceneNode));
}

void Game::onObjectLocationChanged(SpatialObject &object) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    auto sceneNode = scene.modelByExternalRef(&object);
    if (!sceneNode) {
        return;
    }
    auto transform = glm::rotate(
        glm::translate(object.position()),
        object.facing(),
        glm::vec3 {0.0f, 0.0f, 1.0f});
    sceneNode->get().setLocalTransform(std::move(transform));
}

void Game::render() {
    glm::ivec2 screenSize {_options.graphics.width, _options.graphics.height};
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    auto &output = scene.render(screenSize);
    auto &program = _graphicsSvc.shaderRegistry.get(ShaderProgramId::ndcTexture);
    _graphicsSvc.context.useProgram(program);
    _graphicsSvc.context.bindTexture(output);
    _graphicsSvc.meshRegistry.get(MeshName::quadNDC).draw();
}

void Game::pause(bool pause) {
    _paused.store(pause, std::memory_order::memory_order_release);
}

void Game::quit() {
    _quit.store(true, std::memory_order::memory_order_release);
}

void Game::logicThreadFunc() {
    setThreadName(kLogicThreadName);
    _ticks = _systemSvc.clock.millis();

    while (!_quit.load(std::memory_order::memory_order_acquire)) {
        bool paused = _paused.load(std::memory_order::memory_order_acquire);
        if (paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds {100});
            continue;
        }
        uint32_t ticks = _systemSvc.clock.millis();
        float dt = (ticks - _ticks) / 1000.0f;
        _ticks = ticks;
        _profiler.measure(kLogicThreadName, 0, [this, &dt]() {
            if (_module) {
                _module->get().update(dt);
                collectEvents();
            }
            std::queue<AsyncTask> tasks;
            {
                std::lock_guard<std::mutex> lock {_logicTasksMutex};
                std::swap(tasks, _logicTasks);
            }
            while (!tasks.empty()) {
                auto task = tasks.front();
                tasks.pop();
                task();
            }
        });
    }
}

void Game::collectEvents() {
    std::lock_guard<std::mutex> lock {_eventsMutex};

    auto &module = _module->get();
    for (auto &event : module.events()) {
        _events.push_back(std::move(event));
    }
    module.clearEvents();

    auto &area = module.area();
    for (auto &event : area.events()) {
        _events.push_back(std::move(event));
    }
    area.clearEvents();

    for (auto &object : area.objects()) {
        for (auto &event : object.get().events()) {
            _events.push_back(std::move(event));
        }
        object.get().clearEvents();
    }
}

void Game::runOnLogicThread(AsyncTask task) {
    std::lock_guard<std::mutex> lock {_logicTasksMutex};
    _logicTasks.push(std::move(task));
}

void Game::startModule(const std::string &name) {
    _module = loadModule(name);
}

Module &Game::loadModule(const std::string &name) {
    auto ifo = _resourceSvc.gffs.get("module", ResType::Ifo);
    if (!ifo) {
        throw ResourceNotFoundException("Module IFO not found: " + name);
    }
    auto parsedIFO = parseIFO(*ifo);
    auto &module = newModule(parsedIFO.Mod_Tag);
    module.load(parsedIFO);
    return module;
}

Area &Game::loadArea(const std::string &name) {
    auto are = _resourceSvc.gffs.get(name, ResType::Are);
    if (!are) {
        throw ResourceNotFoundException("Area ARE not found: " + name);
    }
    auto parsedARE = parseARE(*are);
    auto git = _resourceSvc.gffs.get(name, ResType::Git);
    if (!git) {
        throw ResourceNotFoundException("Area GIT not found: " + name);
    }
    auto parsedGIT = parseGIT(*git);
    auto lyt = _resourceSvc.layouts.get(name);
    if (!lyt) {
        throw ResourceNotFoundException("Area LYT not found: " + name);
    }
    auto vis = _resourceSvc.visibilities.get(name);
    if (!vis) {
    }
    auto pth = _resourceSvc.paths.get(name);
    if (!pth) {
    }
    auto &area = newArea(parsedARE.Tag);
    area.load(parsedARE, parsedGIT, *lyt, *vis, *pth);
    return area;
}

Camera &Game::loadCamera() {
    auto &camera = newCamera("");
    return camera;
}

Creature &Game::loadCreature(const resource::ResRef &tmplt) {
    auto utc = _resourceSvc.gffs.get(tmplt.value(), ResType::Utc);
    if (!utc) {
        throw ResourceNotFoundException("Creature UTC not found: " + tmplt.value());
    }
    auto parsedUTC = parseUTC(*utc);
    auto &creature = newCreature(parsedUTC.Tag);
    auto appearance = _resourceSvc.twoDas.get("appearance");
    if (!appearance) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }
    auto heads = _resourceSvc.twoDas.get("heads");
    if (!heads) {
        throw ResourceNotFoundException("heads 2DA not found");
    }
    creature.load(parsedUTC, *appearance, *heads);
    return creature;
}

Creature &Game::loadCreature(ObjectTag tag, PortraitId portraitId) {
    auto &creature = newCreature(std::move(tag));
    auto portraits = _resourceSvc.twoDas.get("portraits");
    if (!portraits) {
        throw ResourceNotFoundException("portraits 2DA not found");
    }
    auto appearanceId = portraits->getInt(portraitId, "appearancenumber");
    auto appearance = _resourceSvc.twoDas.get("appearance");
    if (!appearance) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }
    auto heads = _resourceSvc.twoDas.get("heads");
    if (!heads) {
        throw ResourceNotFoundException("heads 2DA not found");
    }
    creature.load(appearanceId, *appearance, *heads);
    return creature;
}

Door &Game::loadDoor(const resource::ResRef &tmplt) {
    auto utd = _resourceSvc.gffs.get(tmplt.value(), ResType::Utd);
    if (!utd) {
        throw ResourceNotFoundException("Door UTD not found: " + tmplt.value());
    }
    auto parsedUTD = parseUTD(*utd);
    auto &door = newDoor(parsedUTD.Tag);
    auto genericDoors = _resourceSvc.twoDas.get("genericdoors");
    if (!genericDoors) {
        throw ResourceNotFoundException("genericdoors 2DA not found");
    }
    door.load(parsedUTD, *genericDoors);
    return door;
}

Encounter &Game::loadEncounter(const resource::ResRef &tmplt) {
    auto ute = _resourceSvc.gffs.get(tmplt.value(), ResType::Ute);
    if (!ute) {
        throw ResourceNotFoundException("Encounter UTE not found: " + tmplt.value());
    }
    auto parsedUTE = parseUTE(*ute);
    auto &encounter = newEncounter(parsedUTE.Tag);
    return encounter;
}

Placeable &Game::loadPlaceable(const resource::ResRef &tmplt) {
    auto utp = _resourceSvc.gffs.get(tmplt.value(), ResType::Utp);
    if (!utp) {
        throw ResourceNotFoundException("Placeable UTP not found: " + tmplt.value());
    }
    auto parsedUTP = parseUTP(*utp);
    auto &placeable = newPlaceable(parsedUTP.Tag);
    auto placeables = _resourceSvc.twoDas.get("placeables");
    if (!placeables) {
        throw ResourceNotFoundException("placeables 2DA not found");
    }
    placeable.load(parsedUTP, *placeables);
    return placeable;
}

Sound &Game::loadSound(const resource::ResRef &tmplt) {
    auto uts = _resourceSvc.gffs.get(tmplt.value(), ResType::Uts);
    if (!uts) {
        throw ResourceNotFoundException("Sound UTS not found: " + tmplt.value());
    }
    auto parsedUTS = parseUTS(*uts);
    auto &sound = newSound(parsedUTS.Tag);
    return sound;
}

Store &Game::loadStore(const resource::ResRef &tmplt) {
    auto utm = _resourceSvc.gffs.get(tmplt.value(), ResType::Utm);
    if (!utm) {
        throw ResourceNotFoundException("Store UTM not found: " + tmplt.value());
    }
    auto parsedUTM = parseUTM(*utm);
    auto &store = newStore(parsedUTM.Tag);
    return store;
}

Trigger &Game::loadTrigger(const resource::ResRef &tmplt) {
    auto utt = _resourceSvc.gffs.get(tmplt.value(), ResType::Utt);
    if (!utt) {
        throw ResourceNotFoundException("Trigger UTT not found: " + tmplt.value());
    }
    auto parsedUTT = parseUTT(*utt);
    auto &trigger = newTrigger(parsedUTT.Tag);
    return trigger;
}

Waypoint &Game::loadWaypoint(const resource::ResRef &tmplt) {
    auto utw = _resourceSvc.gffs.get(tmplt.value(), ResType::Utw);
    if (!utw) {
        throw ResourceNotFoundException("Waypoint UTW not found: " + tmplt.value());
    }
    auto parsedUTW = parseUTW(*utw);
    auto &waypoint = newWaypoint(parsedUTW.Tag);
    return waypoint;
}

Area &Game::newArea(ObjectTag tag) {
    auto object = std::make_unique<Area>(_nextObjectId++, std::move(tag), *this);
    auto &area = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return area;
}

Camera &Game::newCamera(ObjectTag tag) {
    auto object = std::make_unique<Camera>(_nextObjectId++, std::move(tag));
    auto &camera = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return camera;
}

Creature &Game::newCreature(ObjectTag tag) {
    auto object = std::make_unique<Creature>(_nextObjectId++, std::move(tag));
    auto &creature = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return creature;
}

Door &Game::newDoor(ObjectTag tag) {
    auto object = std::make_unique<Door>(_nextObjectId++, std::move(tag));
    auto &door = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return door;
}

Encounter &Game::newEncounter(ObjectTag tag) {
    auto object = std::make_unique<Encounter>(_nextObjectId++, std::move(tag));
    auto &encounter = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return encounter;
}

Item &Game::newItem(ObjectTag tag) {
    auto object = std::make_unique<Item>(_nextObjectId++, std::move(tag));
    auto &item = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return item;
}

Module &Game::newModule(ObjectTag tag) {
    auto object = std::make_unique<Module>(_nextObjectId++, std::move(tag), *this);
    auto &module = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return module;
}

Placeable &Game::newPlaceable(ObjectTag tag) {
    auto object = std::make_unique<Placeable>(_nextObjectId++, std::move(tag));
    auto &placeable = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return placeable;
}

Sound &Game::newSound(ObjectTag tag) {
    auto object = std::make_unique<Sound>(_nextObjectId++, std::move(tag));
    auto &sound = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return sound;
}

Store &Game::newStore(ObjectTag tag) {
    auto object = std::make_unique<Store>(_nextObjectId++, std::move(tag));
    auto &store = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return store;
}

Trigger &Game::newTrigger(ObjectTag tag) {
    auto object = std::make_unique<Trigger>(_nextObjectId++, std::move(tag));
    auto &trigger = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return trigger;
}

Waypoint &Game::newWaypoint(ObjectTag tag) {
    auto object = std::make_unique<Waypoint>(_nextObjectId++, std::move(tag));
    auto &waypoint = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return waypoint;
}

} // namespace neo

} // namespace game

} // namespace reone
