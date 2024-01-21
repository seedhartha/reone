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

namespace graphics {

struct GraphicsServices;

}

namespace resource {

struct ResourceServices;

}

namespace scene {

struct SceneServices;

class CameraSceneNode;

} // namespace scene

namespace game {

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

class CameraController : boost::noncopyable {
public:
    bool handle(const input::Event &event);
    void update(float dt);

    void setSceneNode(scene::CameraSceneNode &sceneNode) {
        _sceneNode = sceneNode;
    }

    void refreshSceneNode();

private:
    struct MovementDirections {
        static constexpr int None = 0;
        static constexpr int Right = 1 << 0;
        static constexpr int Left = 1 << 1;
        static constexpr int Front = 1 << 2;
        static constexpr int Back = 1 << 3;
        static constexpr int Up = 1 << 4;
        static constexpr int Down = 1 << 5;
    };

    glm::vec3 _position {0.0f};
    float _facing {0.0f};
    float _pitch {glm::half_pi<float>()};
    int _movementDir {0};

    std::optional<std::reference_wrapper<scene::CameraSceneNode>> _sceneNode;
};

class Game : public IAreaLoader, public IAreaObjectLoader, boost::noncopyable {
public:
    Game(OptionsView &options,
         graphics::GraphicsServices &graphicsSvc,
         resource::ResourceServices &resourceSvc,
         scene::SceneServices &sceneSvc) :
        _options(options),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc),
        _sceneSvc(sceneSvc) {
    }

    ~Game() {
        deinit();
    }

    void init();
    void deinit();

    bool handle(const input::Event &event);
    void update(float dt);
    void render();

    // Module

    void startModule(const std::string &name);

    Module &loadModule(const std::string &name);

    std::optional<std::reference_wrapper<Module>> module() const {
        return _module;
    }

    // END Module

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
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;
    scene::SceneServices &_sceneSvc;

    bool _inited {false};

    std::optional<std::reference_wrapper<scene::CameraSceneNode>> _cameraSceneNode;
    CameraController _cameraController;

    ObjectId _nextObjectId {2};
    std::list<std::unique_ptr<Object>> _objects;
    std::optional<std::reference_wrapper<Module>> _module;
};

} // namespace neo

} // namespace game

} // namespace reone
