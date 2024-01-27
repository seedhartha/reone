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
#include "reone/resource/parser/2da/appearance.h"
#include "reone/resource/parser/2da/genericdoors.h"
#include "reone/resource/parser/2da/heads.h"
#include "reone/resource/parser/2da/placeables.h"
#include "reone/resource/parser/2da/portraits.h"
#include "reone/resource/parser/2da/surfacemat.h"
#include "reone/resource/parser/gff/are.h"
#include "reone/resource/parser/gff/git.h"
#include "reone/resource/parser/gff/ifo.h"
#include "reone/resource/parser/gff/utc.h"
#include "reone/resource/parser/gff/utd.h"
#include "reone/resource/parser/gff/ute.h"
#include "reone/resource/parser/gff/utm.h"
#include "reone/resource/parser/gff/utp.h"
#include "reone/resource/parser/gff/uts.h"
#include "reone/resource/parser/gff/utt.h"
#include "reone/resource/parser/gff/utw.h"
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

void Game::init() {
    checkThat(!_inited, "Must not be initialized");

    auto surfacematRaw = _resourceSvc.twoDas.get("surfacemat");
    if (!surfacematRaw) {
        throw ResourceNotFoundException("surfacemat 2DA not found");
    }
    auto surfacemat = parseSurfacematTwoDA(*surfacematRaw);
    for (size_t i = 0; i < surfacemat.rows.size(); ++i) {
        const auto &row = surfacemat.rows[i];
        if (row.walk) {
            _walkSurfaceMaterials.insert(i);
        }
        if (row.walkcheck) {
            _walkcheckSurfaceMaterials.insert(i);
        }
        if (row.lineofsight) {
            _lineOfSightSurfaceMaterials.insert(i);
        }
    }

    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    scene.setLineOfSightSurfaces(_lineOfSightSurfaceMaterials);
    scene.setUpdateRoots(true);

    _actionExecutor = std::make_unique<ActionExecutor>();
    _eventHandler = std::make_unique<EventHandler>(*this, _options.graphics, _resourceSvc, _sceneSvc);

    _playerCameraController = std::make_unique<PlayerCameraController>(scene);
    _selectionController = std::make_unique<SelectionController>(_options.graphics, scene);

    startModule("end_m01aa");

    if (_module) {
        auto &module = _module->get();
        auto &pc = loadCreature("", 23);
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
                _module->get().update(dt);
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

bool Game::executeAction(Object &subject, const Action &action, float dt) {
    if (subject.type() == ObjectType::Creature) {
        auto &creature = static_cast<Creature &>(subject);
        if (action.type == ActionType::MoveToPoint) {
            return executeMoveToPoint(creature, action, dt);
        }
    }
    return false;
}

bool Game::executeMoveToPoint(Creature &subject, const Action &action, float dt) {
    glm::vec3 delta = action.location.position - subject.position();
    glm::vec2 delta2D = delta;
    auto distance2D = glm::length(delta2D);
    if (distance2D < 1.0f) {
        subject.setMoveType(Creature::MoveType::None);
        return true;
    }
    glm::vec3 dir = glm::normalize(delta);
    glm::vec2 dir2D = delta2D / distance2D;
    glm::vec2 oldPos2D = subject.position();
    glm::vec2 newPos2D = oldPos2D + kPlayerMoveRate * dir2D * dt;
    subject.setFacingPoint(action.location.position);

    std::vector<ObjectWalkmesh> walkmeshes;
    auto &area = _module->get().area();
    for (const auto &room : area.rooms()) {
        auto walkmesh = _resourceSvc.walkmeshes.get(room.model, ResType::Wok);
        if (walkmesh) {
            walkmeshes.emplace_back(area.id(), *walkmesh);
        }
    }
    for (const auto &door : area.doors()) {
        auto walkmeshResRef = door.get().modelName().value();
        DoorWalkmeshType walkmeshType;
        if (door.get().isClosed()) {
            walkmeshResRef += "0";
            walkmeshType = DoorWalkmeshType::Closed;
        } else {
            walkmeshResRef += "1";
            walkmeshType = DoorWalkmeshType::Open1;
        }
        auto walkmesh = _resourceSvc.walkmeshes.get(walkmeshResRef, ResType::Dwk);
        if (walkmesh) {
            walkmeshes.emplace_back(door.get().id(), *walkmesh, walkmeshType);
        }
    }
    for (const auto &placeable : area.placeables()) {
        auto walkmeshResRef = placeable.get().modelName().value();
        auto walkmesh = _resourceSvc.walkmeshes.get(walkmeshResRef, ResType::Pwk);
        if (walkmesh) {
            walkmeshes.emplace_back(placeable.get().id(), *walkmesh);
        }
    }

    // TODO: extract method
    // First pass: Z coord and floor obstructions
    std::optional<float> zCoord;
    for (const auto &ow : walkmeshes) {
        const auto &object = _idToObject.at(ow.objectId);
        const auto &walkmesh = ow.walkmesh;
        glm::mat4 toLocal {1.0f};
        if (object.get().type() == ObjectType::Area) {
            if (!walkmesh.contains(oldPos2D) &&
                !walkmesh.contains(newPos2D)) {
                continue;
            }
        } else {
            const auto &spatial = static_cast<SpatialObject &>(object.get());
            float distance = glm::distance(subject.position(), spatial.position());
            if (distance > 4.0f) {
                continue;
            }
            toLocal = glm::inverse(glm::rotate(
                glm::translate(spatial.position()),
                spatial.facing(),
                glm::vec3 {0.0f, 0.0f, 1.0f}));
        }
        glm::vec3 originLocal = toLocal * glm::vec4 {newPos2D, 1000.0f, 1.0f};
        glm::vec3 dirLocal = toLocal * glm::vec4 {0.0f, 0.0f, -1.0f, 0.0f};
        float distance;
        auto face = walkmesh.raycast(
            _walkcheckSurfaceMaterials,
            originLocal,
            dirLocal,
            std::numeric_limits<float>::max(),
            distance);
        if (face) {
            bool walkable = _walkSurfaceMaterials.count(face->material);
            if (!walkable) {
                subject.setMoveType(Creature::MoveType::None);
                return false;
            }
            zCoord = 1000.0f - distance;
        }
    }
    if (!zCoord) {
        return false;
    }
    // Second pass: wall obstructions
    for (const auto &ow : walkmeshes) {
        const auto &object = _idToObject.at(ow.objectId);
        const auto &walkmesh = ow.walkmesh;
        glm::mat4 toLocal {1.0f};
        if (object.get().type() == ObjectType::Area) {
            if (!walkmesh.contains(oldPos2D) &&
                !walkmesh.contains(newPos2D)) {
                continue;
            }
        } else {
            const auto &spatial = static_cast<SpatialObject &>(object.get());
            float distance = glm::distance(subject.position(), spatial.position());
            if (distance > 4.0f) {
                continue;
            }
            toLocal = glm::inverse(glm::rotate(
                glm::translate(spatial.position()),
                spatial.facing(),
                glm::vec3 {0.0f, 0.0f, 1.0f}));
        }
        glm::vec3 originLocal = toLocal * glm::vec4 {subject.position(), 1.0f};
        glm::vec3 dirLocal = toLocal * glm::vec4 {dir, 0.0f};
        float distance;
        auto face = walkmesh.raycast(
            _walkcheckSurfaceMaterials,
            originLocal + glm::vec3 {0.0f, 0.0f, 0.5f},
            dirLocal,
            kPlayerMoveRate * dt,
            distance);
        if (face && _walkSurfaceMaterials.count(face->material) == 0) {
            subject.setMoveType(Creature::MoveType::None);
            return false;
        }
    }
    subject.setMoveType(Creature::MoveType::Run);
    subject.setPosition(glm::vec3 {newPos2D, *zCoord});
    return false;
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
    auto appearanceRaw = _resourceSvc.twoDas.get("appearance");
    if (!appearanceRaw) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }
    auto appearance = parseAppearanceTwoDA(*appearanceRaw);
    auto headsRaw = _resourceSvc.twoDas.get("heads");
    if (!headsRaw) {
        throw ResourceNotFoundException("heads 2DA not found");
    }
    auto heads = parseHeadsTwoDA(*headsRaw);
    creature.load(parsedUTC, appearance, heads);
    return creature;
}

Creature &Game::loadCreature(ObjectTag tag, PortraitId portraitId) {
    auto &creature = newCreature(std::move(tag));
    auto portraits = _resourceSvc.twoDas.get("portraits");
    if (!portraits) {
        throw ResourceNotFoundException("portraits 2DA not found");
    }
    auto appearanceRaw = _resourceSvc.twoDas.get("appearance");
    if (!appearanceRaw) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }
    auto appearance = parseAppearanceTwoDA(*appearanceRaw);
    auto headsRaw = _resourceSvc.twoDas.get("heads");
    if (!headsRaw) {
        throw ResourceNotFoundException("heads 2DA not found");
    }
    auto heads = parseHeadsTwoDA(*headsRaw);
    auto portraitsRow = parsePortraitsTwoDARow(*portraits, portraitId);
    creature.load(*portraitsRow.appearancenumber, appearance, heads);
    return creature;
}

Door &Game::loadDoor(const resource::ResRef &tmplt) {
    auto utd = _resourceSvc.gffs.get(tmplt.value(), ResType::Utd);
    if (!utd) {
        throw ResourceNotFoundException("Door UTD not found: " + tmplt.value());
    }
    auto parsedUTD = parseUTD(*utd);
    auto &door = newDoor(parsedUTD.Tag);
    auto genericDoorsRaw = _resourceSvc.twoDas.get("genericdoors");
    if (!genericDoorsRaw) {
        throw ResourceNotFoundException("genericdoors 2DA not found");
    }
    auto genericDoors = parseGenericdoorsTwoDA(*genericDoorsRaw);
    door.load(parsedUTD, genericDoors);
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
    auto placeablesRaw = _resourceSvc.twoDas.get("placeables");
    if (!placeablesRaw) {
        throw ResourceNotFoundException("placeables 2DA not found");
    }
    auto placeables = parsePlaceablesTwoDA(*placeablesRaw);
    placeable.load(parsedUTP, placeables);
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
    auto object = std::make_unique<Area>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this,
        *this);
    auto &area = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return area;
}

Camera &Game::newCamera(ObjectTag tag) {
    auto object = std::make_unique<Camera>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &camera = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return camera;
}

Creature &Game::newCreature(ObjectTag tag) {
    auto object = std::make_unique<Creature>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &creature = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return creature;
}

Door &Game::newDoor(ObjectTag tag) {
    auto object = std::make_unique<Door>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &door = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return door;
}

Encounter &Game::newEncounter(ObjectTag tag) {
    auto object = std::make_unique<Encounter>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &encounter = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return encounter;
}

Item &Game::newItem(ObjectTag tag) {
    auto object = std::make_unique<Item>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &item = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return item;
}

Module &Game::newModule(ObjectTag tag) {
    auto object = std::make_unique<Module>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this,
        *this);
    auto &module = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return module;
}

Placeable &Game::newPlaceable(ObjectTag tag) {
    auto object = std::make_unique<Placeable>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &placeable = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return placeable;
}

Sound &Game::newSound(ObjectTag tag) {
    auto object = std::make_unique<Sound>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &sound = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return sound;
}

Store &Game::newStore(ObjectTag tag) {
    auto object = std::make_unique<Store>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &store = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return store;
}

Trigger &Game::newTrigger(ObjectTag tag) {
    auto object = std::make_unique<Trigger>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &trigger = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return trigger;
}

Waypoint &Game::newWaypoint(ObjectTag tag) {
    auto object = std::make_unique<Waypoint>(
        _nextObjectId++,
        std::move(tag),
        *this,
        *this);
    auto &waypoint = *object;
    _objects.push_back(std::move(object));
    auto &inserted = *_objects.back();
    _idToObject.insert({inserted.id(), inserted});
    return waypoint;
}

} // namespace neo

} // namespace game

} // namespace reone
