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
#include "reone/graphics/uniforms.h"
#include "reone/resource/di/services.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/parser/2da/surfacemat.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/layouts.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/paths.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/provider/visibilities.h"
#include "reone/resource/provider/walkmeshes.h"
#include "reone/scene/collision.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/system/checkutil.h"
#include "reone/system/clock.h"
#include "reone/system/di/services.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/exception/validation.h"
#include "reone/system/logutil.h"
#include "reone/system/threadutil.h"

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::resource::generated;
using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

static constexpr float kCameraPitch = 84.0f;

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

void Game::init() {
    checkThat(!_inited, "Must not be initialized");

    std::set<uint32_t> walkSurfaceMaterials;
    std::set<uint32_t> walkcheckSurfaceMaterials;
    std::set<uint32_t> lineOfSightSurfaceMaterials;
    auto surfacematRaw = _resourceSvc.twoDas.get("surfacemat");
    if (!surfacematRaw) {
        throw ResourceNotFoundException("surfacemat 2DA not found");
    }
    auto surfacemat = parseSurfacematTwoDA(*surfacematRaw);
    for (size_t i = 0; i < surfacemat.rows.size(); ++i) {
        const auto &row = surfacemat.rows[i];
        if (row.walk) {
            walkSurfaceMaterials.insert(i);
        }
        if (row.walkcheck) {
            walkcheckSurfaceMaterials.insert(i);
        }
        if (row.lineofsight) {
            lineOfSightSurfaceMaterials.insert(i);
        }
    }

    _objectRepository = std::make_unique<ObjectRepository>(*this);
    _objectLoader = std::make_unique<ObjectLoader>(*_objectRepository, _resourceSvc);
    _eventHandler = std::make_unique<EventHandler>(*_objectRepository, _options.graphics, _resourceSvc, _sceneSvc);

    _actionExecutor = std::make_unique<ActionExecutor>(*_objectRepository, _resourceSvc);
    _actionExecutor->setWalkSurfaceMaterials(walkSurfaceMaterials);
    _actionExecutor->setWalkcheckSurfaceMaterials(walkcheckSurfaceMaterials);

    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    scene.setLineOfSightSurfaces(lineOfSightSurfaceMaterials);
    scene.setUpdateRoots(true);

    _playerCameraController = std::make_unique<PlayerCameraController>(scene);
    _selectionController = std::make_unique<SelectionController>(_options.graphics, scene);

    startModule("end_m01aa");

    if (_module) {
        auto &module = _module->get();
        _actionExecutor->setModule(module);

        auto &pc = _objectLoader->loadCreature("", 23);
        _pc = pc;
        pc.setPosition(module.entryPosition());
        pc.setFacing(module.entryFacing());
        module.area().add(pc);
        _playerCameraController->setPlayer(pc);
        _playerCameraController->setGameLogicExecutor(std::bind(&Game::runOnLogicThread, this, std::placeholders::_1));

        auto camera = scene.newCamera();
        float aspect = _options.graphics.width / static_cast<float>(_options.graphics.height);
        camera->setPerspectiveProjection(glm::radians(55.0f), aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);
        _playerCameraController->setCameraSceneNode(*camera);
        _selectionController->setCamera(*camera);
        _playerCameraController->setCameraFacing(module.entryFacing());
        _playerCameraController->setCameraPitch(glm::radians(kCameraPitch));
        _playerCameraController->refreshCamera();
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
    if (_playerCameraController->handle(event)) {
        return true;
    }
    auto oldSelected = _selectionController->selectedObject();
    if (_selectionController->handle(event)) {
        auto newSelected = _selectionController->selectedObject();
        if (newSelected && (!oldSelected || newSelected->get() != oldSelected->get())) {
            if (newSelected->get().type() == ObjectType::Door) {
                auto &door = static_cast<Door &>(newSelected->get());
                door.open();
                _selectionController->clear();
            }
        }
        return true;
    }
    switch (event.type) {
    default:
        break;
    }
    return false;
}

void Game::update(float dt) {
    handleEvents();
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    scene.update(dt);
    _playerCameraController->update(dt);
}

void Game::handleEvents() {
    EventList events;
    {
        std::lock_guard<std::mutex> lock {_eventsMutex};
        std::swap(events, _eventsFrontBuf);
    }
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    for (const auto &event : events) {
        _eventHandler.get()->handle(event);
        if (event.type == EventType::ObjectStateChanged &&
            event.object.state == ObjectState::Loaded &&
            (_pc && event.object.objectId == _pc->get().id())) {
            auto sceneNode = scene.modelByExternalRef(&_pc->get());
            if (sceneNode) {
                _playerCameraController->setPlayerSceneNode(sceneNode->get());
            }
        }
    }
}

void Game::render() {
    glm::ivec2 screenSize {_options.graphics.width, _options.graphics.height};
    auto &scene = _sceneSvc.graphs.get(kSceneMain);

    // Scene
    auto &output = scene.render(screenSize);
    auto &program = _graphicsSvc.shaderRegistry.get(ShaderProgramId::ndcTexture);
    _graphicsSvc.context.useProgram(program);
    _graphicsSvc.context.bindTexture(output);
    _graphicsSvc.meshRegistry.get(MeshName::quadNDC).draw();

    // Reticles
    auto hoveredObject = _selectionController->hoveredObject();
    auto selectedObject = _selectionController->selectedObject();
    if ((hoveredObject || selectedObject) && _cameraSceneNode) {
        _graphicsSvc.context.useProgram(_graphicsSvc.shaderRegistry.get(ShaderProgramId::mvpTexture));
        _graphicsSvc.uniforms.setGlobals([&screenSize](auto &globals) {
            globals.reset();
            globals.projection = glm::ortho(0.0f, static_cast<float>(screenSize.x),
                                            static_cast<float>(screenSize.y), 0.0f,
                                            0.0f, 1.0f);
        });
        _graphicsSvc.context.withBlendMode(BlendMode::Additive, [this, &screenSize, &scene, &hoveredObject, &selectedObject]() {
            const auto &camera = *_cameraSceneNode->get().camera();
            if (hoveredObject) {
                auto model = scene.modelByExternalRef(&hoveredObject->get());
                if (model) {
                    auto reticle = _resourceSvc.textures.get("friendlyreticle", TextureUsage::GUI);
                    auto objectWorld = model->get().getWorldCenterOfAABB();
                    auto objectNDC = camera.projection() * camera.view() * glm::vec4 {objectWorld, 1.0f};
                    if (objectNDC.w > 0.0f) {
                        objectNDC /= objectNDC.w;
                        glm::vec3 objectScreen {screenSize.x * 0.5f * (objectNDC.x + 1.0f),
                                                screenSize.y * (1.0f - 0.5f * (objectNDC.y + 1.0f)),
                                                0.0f};
                        auto transform = glm::scale(
                            glm::translate(objectScreen - 0.5f * glm::vec3 {reticle->width(), reticle->height(), 0.0f}),
                            glm::vec3 {reticle->width(), reticle->height(), 1.0f});
                        _graphicsSvc.context.bindTexture(*reticle);
                        _graphicsSvc.uniforms.setLocals([&transform](auto &locals) {
                            locals.reset();
                            locals.model = std::move(transform);
                        });
                        _graphicsSvc.meshRegistry.get(MeshName::quad).draw();
                    }
                }
            }
            if (selectedObject) {
                auto model = scene.modelByExternalRef(&selectedObject->get());
                if (model) {
                    auto reticle2 = _resourceSvc.textures.get("friendlyreticle2", TextureUsage::GUI);
                    auto objectWorld = model->get().getWorldCenterOfAABB();
                    auto objectNDC = camera.projection() * camera.view() * glm::vec4 {objectWorld, 1.0f};
                    if (objectNDC.w > 0.0f) {
                        objectNDC /= objectNDC.w;
                        glm::vec3 objectScreen {screenSize.x * 0.5f * (objectNDC.x + 1.0f),
                                                screenSize.y * (1.0f - 0.5f * (objectNDC.y + 1.0f)),
                                                0.0f};
                        auto transform = glm::scale(
                            glm::translate(objectScreen - 0.5f * glm::vec3 {reticle2->width(), reticle2->height(), 0.0f}),
                            glm::vec3 {reticle2->width(), reticle2->height(), 1.0f});
                        _graphicsSvc.context.bindTexture(*reticle2);
                        _graphicsSvc.uniforms.setLocals([&transform](auto &locals) {
                            locals.reset();
                            locals.model = std::move(transform);
                        });
                        _graphicsSvc.meshRegistry.get(MeshName::quad).draw();
                    }
                }
            }
        });
    }
}

void Game::pause(bool pause) {
    _paused.store(pause, std::memory_order::memory_order_release);
}

void Game::quit() {
    _quit.store(true, std::memory_order::memory_order_release);
}

void Game::logicThreadFunc() {
    setThreadName(kLogicThreadName);
    _ticks = _systemSvc.clock.micros();

    while (!_quit.load(std::memory_order::memory_order_acquire)) {
        bool paused = _paused.load(std::memory_order::memory_order_acquire);
        if (paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds {100});
            continue;
        }
        uint64_t ticks = _systemSvc.clock.micros();
        float dt = (ticks - _ticks) / 10e5f;
        _ticks = ticks;
        _profiler.measure(kLogicThreadName, 0, [this, &dt]() {
            if (_module) {
                _module->get().update(*_actionExecutor, dt);
                flushEvents();
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

void Game::flushEvents() {
    std::lock_guard<std::mutex> lock {_eventsMutex};
    for (auto &event : _eventsBackBuf) {
        _eventsFrontBuf.push_back(std::move(event));
    }
    _eventsBackBuf.clear();
}

void Game::runOnLogicThread(AsyncTask task) {
    std::lock_guard<std::mutex> lock {_logicTasksMutex};
    _logicTasks.push(std::move(task));
}

void Game::startModule(const std::string &name) {
    _module = _objectLoader->loadModule(name);
}

} // namespace neo

} // namespace game

} // namespace reone
