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

namespace reone {

namespace graphics {

struct GraphicsOptions;

}

namespace resource {

struct ResourceServices;

}

namespace scene {

struct SceneServices;

}

namespace game {

namespace neo {

enum class DoorState;

struct Event;

class Area;
class Creature;
class Door;
class Module;
class Object;
class Placeable;
class SpatialObject;

class EventHandler : boost::noncopyable {
public:
    EventHandler(std::unique_lock<std::mutex> &sceneLock,
                 graphics::GraphicsOptions &graphicsOpt,
                 resource::ResourceServices &resourceSvc,
                 scene::SceneServices &sceneSvc) :
        _sceneLock(sceneLock),
        _graphicsOpt(graphicsOpt),
        _resourceSvc(resourceSvc),
        _sceneSvc(sceneSvc) {
    }

    void handle(const Event &event);

    void setModule(Module &module) {
        _module = module;
    }

private:
    std::unique_lock<std::mutex> &_sceneLock;
    graphics::GraphicsOptions &_graphicsOpt;
    resource::ResourceServices &_resourceSvc;
    scene::SceneServices &_sceneSvc;

    std::optional<std::reference_wrapper<Module>> _module;

    void onAreaLoaded(Area &area);
    void onCreatureLoaded(Creature &creature);
    void onDoorLoaded(Door &door);
    void onPlaceableLoaded(Placeable &placeable);
    void onObjectLocationChanged(SpatialObject &object);
    void onObjectAnimationReset(Object &object, const std::string &animName);
    void onObjectFireForgetAnimationFired(Object &object, const std::string &animName);
    void onDoorStateChanged(Door &door, DoorState state);
};

} // namespace neo

} // namespace game

} // namespace reone
