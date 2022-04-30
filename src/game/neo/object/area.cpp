/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "area.h"

#include "../../../common/exception/validation.h"
#include "../../../resource/gffs.h"
#include "../../../resource/gff.h"
#include "../../../resource/services.h"

#include "../../layouts.h"
#include "../../services.h"

#include "camera.h"
#include "creature.h"
#include "door.h"
#include "encounter.h"
#include "placeable.h"
#include "sound.h"
#include "store.h"
#include "trigger.h"
#include "waypoint.h"

using namespace std;

using namespace reone::scene;
using namespace reone::resource;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Area> Area::Loader::load(const std::string &name) {
    info("Loading area " + name);

    // ARE, GIT, LYT

    auto are = _resourceSvc.gffs.get(name, ResourceType::Are);
    if (!are) {
        throw ValidationException("ARE not found: " + name);
    }

    auto git = _resourceSvc.gffs.get(name, ResourceType::Git);
    if (!git) {
        throw ValidationException("GIT not found: " + name);
    }

    auto layout = _gameSvc.layouts.get(name);
    if (!layout) {
        throw ValidationException("LYT not found: " + name);
    }

    // Loaders

    auto roomLoader = Room::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto cameraLoader = Camera::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto creatureLoader = Creature::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto placeableLoader = Placeable::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto doorLoader = Door::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto soundLoader = Sound::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto triggerLoader = Trigger::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto encounterLoader = Encounter::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto waypointLoader = Waypoint::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto storeLoader = Store::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);

    // Rooms

    auto rooms = vector<shared_ptr<Room>>();
    auto areRooms = are->getList("Rooms");
    for (auto &areRoom : areRooms) {
        auto roomName = areRoom->getString("RoomName");
        auto layoutRoom = layout->findByName(roomName);
        rooms.push_back(roomLoader.load(roomName, layoutRoom->position));
    }

    // Main camera

    auto cameraStyle = are->getInt("CameraStyle");
    auto mainCamera = cameraLoader.load(cameraStyle);

    auto area = Area::Builder()
                    .id(_idSeq.nextObjectId())
                    .tag(name)
                    .sceneGraph(&_sceneGraph)
                    .rooms(move(rooms))
                    .mainCamera(move(mainCamera))
                    .build();

    // Creatures

    auto gitCreatures = git->getList("Creature List");
    for (auto &gitCreature : gitCreatures) {
        area->add(creatureLoader.load(*gitCreature));
    }

    // Placeables

    auto gitPlaceables = git->getList("Placeable List");
    for (auto &gitPlaceable : gitPlaceables) {
        area->add(placeableLoader.load(*gitPlaceable));
    }

    // Doors

    auto gitDoors = git->getList("Door List");
    for (auto &gitDoor : gitDoors) {
        area->add(doorLoader.load(*gitDoor));
    }

    // Cameras

    auto gitCameras = git->getList("CameraList");
    for (auto &gitCamera : gitCameras) {
        area->add(cameraLoader.load(*gitCamera));
    }

    // Sounds

    auto gitSounds = git->getList("SoundList");
    for (auto &gitSound : gitDoors) {
        area->add(soundLoader.load(*gitSound));
    }

    // Triggers

    auto gitTriggers = git->getList("TriggerList");
    for (auto &gitTrigger : gitTriggers) {
        area->add(triggerLoader.load(*gitTrigger));
    }

    // Encounters

    auto gitEncounters = git->getList("Encounter List");
    for (auto &gitEncounter : gitEncounters) {
        area->add(encounterLoader.load(*gitEncounter));
    }

    // Waypoints

    auto gitWaypoints = git->getList("WaypointList");
    for (auto &gitWaypoint : gitWaypoints) {
        area->add(waypointLoader.load(*gitWaypoint));
    }

    // Stores

    auto gitStores = git->getList("StoreList");
    for (auto &gitStore : gitStores) {
        area->add(storeLoader.load(*gitStore));
    }

    return move(area);
}

} // namespace neo

} // namespace game

} // namespace reone