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

#include "object/area.h"
#include "object/module.h"

namespace reone {

struct SystemServices;

namespace graphics {

struct GraphicsServices;

}

namespace resource {

struct ResourceServices;

}

namespace scene {

struct SceneServices;

class CameraSceneNode;
class ModelSceneNode;

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

using AsyncTask = std::function<void()>;
using AsyncTaskExecutor = std::function<void(AsyncTask)>;

class PlayerCameraController : boost::noncopyable {
public:
    bool handle(const input::Event &event);
    void update(float dt);

    void setCamera(scene::CameraSceneNode &camera) {
        _camera = camera;
    }

    void setCameraPosition(glm::vec3 position) {
        _cameraPosition = std::move(position);
    }

    void setCameraFacing(float facing) {
        _cameraFacing = facing;
    }

    void setCameraPitch(float pitch) {
        _cameraPitch = pitch;
    }

    void setPlayer(Creature &player) {
        _player = player;
    }

    void setGameLogicExecutor(AsyncTaskExecutor executor) {
        _gameLogicExecutor = std::move(executor);
    }

    void refreshCamera();
    void refreshPlayer();

private:
    struct CommandTypes {
        static constexpr int None = 0;
        static constexpr int MoveCameraRight = 1 << 0;
        static constexpr int MoveCameraLeft = 1 << 1;
        static constexpr int MoveCameraFront = 1 << 2;
        static constexpr int MoveCameraBack = 1 << 3;
        static constexpr int MoveCameraUp = 1 << 4;
        static constexpr int MoveCameraDown = 1 << 5;
        static constexpr int RotateCameraCW = 1 << 6;
        static constexpr int RotateCameraCCW = 1 << 7;
        static constexpr int MovePlayerFront = 1 << 8;
        static constexpr int MovePlayerBack = 1 << 9;
        static constexpr int MovePlayerLeft = 1 << 10;
        static constexpr int MovePlayerRight = 1 << 11;
    };

    glm::vec3 _cameraPosition {0.0f};
    float _cameraFacing {0.0f};
    float _cameraPitch {0.0f};

    glm::vec3 _playerMoveDir {0.0f};

    int _commandMask {0};

    std::optional<std::reference_wrapper<scene::CameraSceneNode>> _camera;
    std::optional<std::reference_wrapper<Creature>> _player;
    std::optional<AsyncTaskExecutor> _gameLogicExecutor;
};

class Game : public IAreaLoader,
             public IAreaObjectLoader,
             public IActionExecutor,
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
    OptionsView &_options;
    SystemServices &_systemSvc;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;
    scene::SceneServices &_sceneSvc;
    IConsole &_console;
    IProfiler &_profiler;

    bool _inited {false};

    uint32_t _ticks {0};
    std::atomic_bool _quit {false};
    std::atomic_bool _paused {false};
    std::thread _logicThread;
    std::queue<AsyncTask> _logicTasks;
    std::mutex _logicTasksMutex;

    std::list<Event> _events;
    std::mutex _eventsMutex;

    ObjectId _nextObjectId {2};
    std::list<std::unique_ptr<Object>> _objects;
    std::map<ObjectId, std::reference_wrapper<Object>> _idToObject;
    std::optional<std::reference_wrapper<Module>> _module;
    std::optional<std::reference_wrapper<Creature>> _pc;

    PlayerCameraController _playerCameraController;
    std::optional<std::reference_wrapper<scene::CameraSceneNode>> _cameraSceneNode;
    std::optional<std::reference_wrapper<scene::ModelSceneNode>> _pickedModel;

    // Logic thread

    void logicThreadFunc();
    void collectEvents();

    void runOnLogicThread(AsyncTask task);

    // END Logic thread

    // Event handling

    void handleEvents();

    void onAreaLoaded(Area &area);
    void onCreatureLoaded(Creature &creature);
    void onDoorLoaded(Door &door);
    void onPlaceableLoaded(Placeable &placeable);
    void onObjectLocationChanged(SpatialObject &object);

    // END Event handling
};

} // namespace neo

} // namespace game

} // namespace reone
