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

#include "reone/game/neo/object/area.h"

#include "reone/game/neo/object/camera.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/encounter.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/game/neo/object/sound.h"
#include "reone/game/neo/object/store.h"
#include "reone/game/neo/object/trigger.h"
#include "reone/game/neo/object/waypoint.h"
#include "reone/resource/layout.h"
#include "reone/resource/path.h"
#include "reone/resource/template/generated/are.h"
#include "reone/resource/template/generated/git.h"
#include "reone/system/exception/validation.h"

using namespace reone::resource;
using namespace reone::resource::generated;

namespace reone {

namespace game {

namespace neo {

void Area::load(const ARE &are,
                const GIT &git,
                const Layout &lyt,
                std::optional<std::reference_wrapper<const resource::Visibility>> vis,
                std::optional<std::reference_wrapper<const resource::Path>> pth) {
    for (auto &areRoom : are.Rooms) {
        auto lytRoom = lyt.findByName(areRoom.RoomName);
        if (!lytRoom) {
            throw ValidationException("Area room not found: " + areRoom.RoomName);
        }
        AreaRoom room;
        room.model = areRoom.RoomName;
        room.position = lytRoom->get().position;
        if (vis) {
            auto visibleRange = vis->get().equal_range(areRoom.RoomName);
            for (auto it = visibleRange.first; it != visibleRange.second; ++it) {
                room.visibleRooms.insert(it->second);
            }
        }
        _rooms.push_back(std::move(room));
    }
    for (auto &gitCamera : git.CameraList) {
        auto &camera = _areaObjectLoader.loadCamera();
        add(camera);
    }
    for (auto &gitCreature : git.Creature_List) {
        auto &creature = _areaObjectLoader.loadCreature(gitCreature.TemplateResRef);
        creature.setPosition({gitCreature.XPosition, gitCreature.YPosition, gitCreature.ZPosition});
        creature.setFacing(-glm::atan(gitCreature.XOrientation, gitCreature.YOrientation));
        add(creature);
    }
    for (auto &gitDoor : git.Door_List) {
        auto &door = _areaObjectLoader.loadDoor(gitDoor.TemplateResRef);
        door.setPosition({gitDoor.X, gitDoor.Y, gitDoor.Z});
        door.setFacing(gitDoor.Bearing);
        add(door);
    }
    for (auto &gitEncounter : git.Encounter_List) {
        auto &encounter = _areaObjectLoader.loadEncounter(gitEncounter.TemplateResRef);
        add(encounter);
    }
    for (auto &gitPlaceable : git.Placeable_List) {
        auto &placeable = _areaObjectLoader.loadPlaceable(gitPlaceable.TemplateResRef);
        placeable.setPosition({gitPlaceable.X, gitPlaceable.Y, gitPlaceable.Z});
        placeable.setFacing(gitPlaceable.Bearing);
        add(placeable);
    }
    for (auto &gitTrigger : git.TriggerList) {
        auto &trigger = _areaObjectLoader.loadTrigger(gitTrigger.TemplateResRef);
        add(trigger);
    }
    for (auto &gitSound : git.SoundList) {
        auto &sound = _areaObjectLoader.loadSound(gitSound.TemplateResRef);
        add(sound);
    }
    for (auto &gitStore : git.StoreList) {
        auto &store = _areaObjectLoader.loadStore(gitStore.ResRef);
        add(store);
    }
    for (auto &gitWaypoint : git.WaypointList) {
        auto &waypoint = _areaObjectLoader.loadWaypoint(gitWaypoint.TemplateResRef);
        add(waypoint);
    }
    _state = ObjectState::Loaded;
}

void Area::add(Camera &camera) {
    _cameras.push_back(camera);
    _objects.push_back(camera);
}

void Area::add(Creature &creature) {
    _creatures.push_back(creature);
    _objects.push_back(creature);
}

void Area::add(Door &door) {
    _doors.push_back(door);
    _objects.push_back(door);
}

void Area::add(Encounter &encounter) {
    _encounters.push_back(encounter);
    _objects.push_back(encounter);
}

void Area::add(Placeable &placeable) {
    _placeables.push_back(placeable);
    _objects.push_back(placeable);
}

void Area::add(Trigger &trigger) {
    _triggers.push_back(trigger);
    _objects.push_back(trigger);
}

void Area::add(Sound &sound) {
    _sounds.push_back(sound);
    _objects.push_back(sound);
}

void Area::add(Store &store) {
    _stores.push_back(store);
    _objects.push_back(store);
}

void Area::add(Waypoint &waypoint) {
    _waypoints.push_back(waypoint);
    _objects.push_back(waypoint);
}

} // namespace neo

} // namespace game

} // namespace reone
