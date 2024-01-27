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

#pragma once

#include "object.h"

#include "reone/input/event.h"

#include "../options.h"

#include "actionexecutor.h"
#include "controller/playercamera.h"
#include "controller/selection.h"
#include "eventhandler.h"
#include "object/area.h"
#include "object/module.h"
#include "objectrepository.h"

namespace reone {

struct SystemServices;

namespace graphics {

struct GraphicsServices;

class Walkmesh;

} // namespace graphics

namespace resource {

struct ResourceServices;

}

namespace scene {

struct SceneServices;

class CameraSceneNode;
class ModelSceneNode;
class WalkmeshSceneNode;

} // namespace scene

namespace game {

class IConsole;
class IProfiler;

namespace neo {

class Camera;
class Creature;
class Door;
class Encounter;
class Item;
class Placeable;
class Sound;
class Store;
class Trigger;
class Waypoint;

class Game : public IObjectRepository,
             public IAreaLoader,
             public IAreaObjectLoader,
             public IActionExecutor,
             public IEventCollector,
             boost::noncopyable {
public:
    Game(OptionsView &options,
         SystemServices &systemSvc,
         graphics::GraphicsServices &graphicsSvc,
         resource::ResourceServices &resourceSvc,
         scene::SceneServices &sceneSvc,
         IConsole &console,
         IProfiler &profiler) :
        _options(options),
        _systemSvc(systemSvc),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc),
        _sceneSvc(sceneSvc),
        _console(console),
        _profiler(profiler) {
    }

    ~Game() {
        deinit();
    }

    void init();
    void deinit();

    bool handle(const input::Event &event);
    void update(float dt);
    void render();

    void pause(bool pause);
    void quit();

    // Module

    void startModule(const std::string &name);

    Module &loadModule(const std::string &name);

    std::optional<std::reference_wrapper<Module>> module() const {
        return _module;
    }

    // END Module

    // Player character

    Creature &loadCreature(ObjectTag tag, PortraitId portraitId);

    // END Player character

    // IObjectRepository

    Object &get(ObjectId objectId) override {
        return find(objectId).value();
    }

    std::optional<std::reference_wrapper<Object>> find(ObjectId objectId) override {
        auto it = _idToObject.find(objectId);
        if (it == _idToObject.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    // END IObjectRepository

    // IAreaLoader

    Area &loadArea(const std::string &name) override;

    // END IAreaLoader

    // IAreaObjectLoader

    Camera &loadCamera() override;
    Creature &loadCreature(const resource::ResRef &tmplt) override;
    Door &loadDoor(const resource::ResRef &tmplt) override;
    Encounter &loadEncounter(const resource::ResRef &tmplt) override;
    Placeable &loadPlaceable(const resource::ResRef &tmplt) override;
    Sound &loadSound(const resource::ResRef &tmplt) override;
    Store &loadStore(const resource::ResRef &tmplt) override;
    Trigger &loadTrigger(const resource::ResRef &tmplt) override;
    Waypoint &loadWaypoint(const resource::ResRef &tmplt) override;

    // END IAreaObjectLoader

    // IActionExecutor

    bool executeAction(Object &subject, const Action &action, float dt) override;

    // END IActionExecutor

    // IEventCollector

    void collectEvent(Event event) override {
        _eventsBackBuf.push_back(std::move(event));
    }

    // END IEventCollector

    // Object factory methods

    Area &newArea(ObjectTag tag);
    Camera &newCamera(ObjectTag tag);
    Creature &newCreature(ObjectTag tag);
    Door &newDoor(ObjectTag tag);
    Encounter &newEncounter(ObjectTag tag);
    Item &newItem(ObjectTag tag);
    Module &newModule(ObjectTag tag);
    Placeable &newPlaceable(ObjectTag tag);
    Sound &newSound(ObjectTag tag);
    Store &newStore(ObjectTag tag);
    Trigger &newTrigger(ObjectTag tag);
    Waypoint &newWaypoint(ObjectTag tag);

    // END Object factory methods

private:
    enum class DoorWalkmeshType {
        Closed,
        Open1,
        Open2
    };

    struct ObjectWalkmesh {
        ObjectId objectId;
        graphics::Walkmesh &walkmesh;
        std::optional<DoorWalkmeshType> doorType;

        ObjectWalkmesh(ObjectId objectId,
                       graphics::Walkmesh &walkmesh,
                       std::optional<DoorWalkmeshType> doorType = std::nullopt) :
            objectId(objectId),
            walkmesh(walkmesh),
            doorType(std::move(doorType)) {
        }
    };

    OptionsView &_options;
    SystemServices &_systemSvc;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;
    scene::SceneServices &_sceneSvc;
    IConsole &_console;
    IProfiler &_profiler;

    bool _inited {false};

    ObjectId _nextObjectId {2};
    std::list<std::unique_ptr<Object>> _objects;
    std::map<ObjectId, std::reference_wrapper<Object>> _idToObject;
    std::optional<std::reference_wrapper<Module>> _module;
    std::optional<std::reference_wrapper<Creature>> _pc;

    std::optional<std::reference_wrapper<scene::CameraSceneNode>> _cameraSceneNode;

    std::set<uint32_t> _walkSurfaceMaterials;
    std::set<uint32_t> _walkcheckSurfaceMaterials;
    std::set<uint32_t> _lineOfSightSurfaceMaterials;

    // Services

    std::unique_ptr<ActionExecutor> _actionExecutor;
    std::unique_ptr<EventHandler> _eventHandler;

    // END Services

    // Controllers

    std::unique_ptr<PlayerCameraController> _playerCameraController;
    std::unique_ptr<SelectionController> _selectionController;

    // END Controllers

    // Logic thread

    uint64_t _ticks {0};
    std::atomic_bool _quit {false};
    std::atomic_bool _paused {false};
    std::thread _logicThread;
    std::queue<AsyncTask> _logicTasks;
    std::mutex _logicTasksMutex;

    std::list<Event> _eventsBackBuf;
    std::list<Event> _eventsFrontBuf;
    std::mutex _eventsMutex;

    // END Logic thread

    void handleEvents();

    // Logic thread

    void logicThreadFunc();
    void flushEvents();

    void runOnLogicThread(AsyncTask task);

    // END Logic thread

    // Action execution

    bool executeMoveToPoint(Creature &subject, const Action &action, float dt);

    // END Action execution
};

} // namespace neo

} // namespace game

} // namespace reone
