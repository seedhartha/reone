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

#include "object/area.h"
#include "object/module.h"

namespace reone {

namespace resource {

struct ResourceServices;

}

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

class Game : public IAreaLoader, public IAreaObjectLoader, boost::noncopyable {
public:
    Game(resource::ResourceServices &resourceSvc) :
        _resourceSvc(resourceSvc) {
    }

    bool handle(const input::Event &event);
    void update(float dt);

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
    resource::ResourceServices &_resourceSvc;

    std::optional<std::reference_wrapper<Module>> _module;

    ObjectId _nextObjectId {2};
    std::list<std::unique_ptr<Object>> _objects;
};

} // namespace neo

} // namespace game

} // namespace reone
