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
#include "objectloader.h"
#include "objectrepository.h"
#include "selectionoverlay.h"

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

class Game : public IEventCollector,
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

    std::optional<std::reference_wrapper<Module>> module() const {
        return _module;
    }

    // END Module

    // IEventCollector

    void collectEvent(Event event) override {
        _eventsBackBuf.push_back(std::move(event));
    }

    // END IEventCollector

private:
    OptionsView &_options;
    SystemServices &_systemSvc;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;
    scene::SceneServices &_sceneSvc;
    IConsole &_console;
    IProfiler &_profiler;

    bool _inited {false};

    std::optional<std::reference_wrapper<Module>> _module;
    std::optional<std::reference_wrapper<Creature>> _pc;

    std::optional<std::reference_wrapper<scene::CameraSceneNode>> _cameraSceneNode;

    // Services

    std::unique_ptr<ObjectLoader> _objectLoader;
    std::unique_ptr<ObjectRepository> _objectRepository;
    std::unique_ptr<ActionExecutor> _actionExecutor;
    std::unique_ptr<EventHandler> _eventHandler;

    // END Services

    // Controllers

    std::unique_ptr<PlayerCameraController> _playerCameraController;
    std::unique_ptr<SelectionController> _selectionController;

    // END Controllers

    // Overlays

    std::unique_ptr<SelectionOverlay> _selectionOverlay;

    // END Overlays

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
};

} // namespace neo

} // namespace game

} // namespace reone
