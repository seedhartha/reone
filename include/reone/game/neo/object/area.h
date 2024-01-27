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

#include "reone/resource/types.h"

#include "../object.h"

namespace reone {

namespace resource {

struct Layout;

class Path;
class ResRef;

namespace generated {

struct ARE;
struct GIT;

} // namespace generated

} // namespace resource

namespace game {

namespace neo {

class Camera;
class Creature;
class Door;
class Encounter;
class Placeable;
class Sound;
class Store;
class Trigger;
class Waypoint;

struct AreaRoom {
    std::string model;
    glm::vec3 position {0.0f};
    std::set<std::string> visibleRooms;
};

using RoomList = std::vector<AreaRoom>;
using CameraList = std::list<std::reference_wrapper<Camera>>;
using CreatureList = std::list<std::reference_wrapper<Creature>>;
using DoorList = std::list<std::reference_wrapper<Door>>;
using EncounterList = std::list<std::reference_wrapper<Encounter>>;
using PlaceableList = std::list<std::reference_wrapper<Placeable>>;
using SoundList = std::list<std::reference_wrapper<Sound>>;
using StoreList = std::list<std::reference_wrapper<Store>>;
using TriggerList = std::list<std::reference_wrapper<Trigger>>;
using WaypointList = std::list<std::reference_wrapper<Waypoint>>;
using ObjectList = std::list<std::reference_wrapper<Object>>;

class IAreaObjectLoader {
public:
    virtual ~IAreaObjectLoader() = default;

    virtual Camera &loadCamera() = 0;
    virtual Creature &loadCreature(const resource::ResRef &tmplt) = 0;
    virtual Door &loadDoor(const resource::ResRef &tmplt) = 0;
    virtual Encounter &loadEncounter(const resource::ResRef &tmplt) = 0;
    virtual Placeable &loadPlaceable(const resource::ResRef &tmplt) = 0;
    virtual Sound &loadSound(const resource::ResRef &tmplt) = 0;
    virtual Store &loadStore(const resource::ResRef &tmplt) = 0;
    virtual Trigger &loadTrigger(const resource::ResRef &tmplt) = 0;
    virtual Waypoint &loadWaypoint(const resource::ResRef &tmplt) = 0;
};

class Area : public Object {
public:
    Area(ObjectId id,
         ObjectTag tag,
         IAreaObjectLoader &areaObjectLoader,
         IActionExecutor &actionExecutor,
         IEventCollector &eventCollector) :
        Object(
            id,
            std::move(tag),
            ObjectType::Area,
            actionExecutor,
            eventCollector),
        _areaObjectLoader(areaObjectLoader) {
    }

    void load(const resource::generated::ARE &are,
              const resource::generated::GIT &git,
              const resource::Layout &lyt,
              std::optional<std::reference_wrapper<const resource::Visibility>> vis,
              std::optional<std::reference_wrapper<const resource::Path>> pth);

    void update(float dt) override {
        for (auto &object : _objects) {
            object.get().update(dt);
        }
    }

    // Objects

    void add(Camera &camera);
    void add(Creature &creature);
    void add(Door &door);
    void add(Encounter &encounter);
    void add(Placeable &placeable);
    void add(Trigger &trigger);
    void add(Sound &sound);
    void add(Store &store);
    void add(Waypoint &waypoint);

    const RoomList &rooms() const {
        return _rooms;
    }

    const CameraList &cameras() const {
        return _cameras;
    }

    const CreatureList &creatures() const {
        return _creatures;
    }

    const DoorList &doors() const {
        return _doors;
    }

    const EncounterList &encounters() const {
        return _encounters;
    }

    const PlaceableList &placeables() const {
        return _placeables;
    }

    const TriggerList &triggers() const {
        return _triggers;
    }

    const SoundList &sounds() const {
        return _sounds;
    }

    const StoreList &stores() const {
        return _stores;
    }

    const WaypointList &waypoints() const {
        return _waypoints;
    }

    const ObjectList &objects() const {
        return _objects;
    }

    // END Objects

private:
    IAreaObjectLoader &_areaObjectLoader;

    RoomList _rooms;
    CameraList _cameras;
    CreatureList _creatures;
    DoorList _doors;
    EncounterList _encounters;
    PlaceableList _placeables;
    TriggerList _triggers;
    SoundList _sounds;
    StoreList _stores;
    WaypointList _waypoints;
    ObjectList _objects;
};

} // namespace neo

} // namespace game

} // namespace reone
