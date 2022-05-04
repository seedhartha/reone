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

#include "../../common/exception/validation.h"
#include "../../resource/gff.h"
#include "../../resource/gffs.h"
#include "../../resource/services.h"
#include "../../scene/graph.h"

#include "../camerastyles.h"
#include "../layouts.h"
#include "../services.h"

#include "creature.h"
#include "door.h"
#include "factory.h"
#include "placeable.h"
#include "room.h"

using namespace std;

using namespace reone::scene;
using namespace reone::resource;

namespace reone {

namespace game {

void Area::load(const string &name) {
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

    // Rooms

    auto areRooms = are->getList("Rooms");
    for (auto &areRoom : areRooms) {
        auto roomName = areRoom->getString("RoomName");
        auto lytRoom = layout->findByName(roomName);
        if (!lytRoom) {
            continue;
        }
        auto room = static_pointer_cast<Room>(_objectFactory.newRoom());
        room->setSceneGraph(_sceneGraph);
        room->loadFromLyt(*lytRoom);
        _rooms.push_back(static_pointer_cast<Room>(room));
    }

    // Main camera

    auto cameraStyleIdx = are->getInt("CameraStyle");
    auto cameraStyle = _gameSvc.cameraStyles.get(cameraStyleIdx);
    float aspect = _graphicsOpt.width / static_cast<float>(_graphicsOpt.height);
    _mainCamera = static_pointer_cast<Camera>(_objectFactory.newCamera());
    _mainCamera->setSceneGraph(_sceneGraph);
    _mainCamera->loadFromStyle(*cameraStyle);

    // Creatures

    auto gitCreatures = git->getList("Creature List");
    for (auto &gitCreature : gitCreatures) {
        auto creature = static_pointer_cast<Creature>(_objectFactory.newCreature());
        creature->setSceneGraph(_sceneGraph);
        creature->loadFromGit(*gitCreature);
        _objects.push_back(move(creature));
    }

    // Placeables

    auto gitPlaceables = git->getList("Placeable List");
    for (auto &gitPlaceable : gitPlaceables) {
        auto placeable = static_pointer_cast<Placeable>(_objectFactory.newPlaceable());
        placeable->setSceneGraph(_sceneGraph);
        placeable->loadFromGit(*gitPlaceable);
        _objects.push_back(move(placeable));
    }

    // Doors

    auto gitDoors = git->getList("Door List");
    for (auto &gitDoor : gitDoors) {
        auto door = static_pointer_cast<Door>(_objectFactory.newDoor());
        door->setSceneGraph(_sceneGraph);
        door->loadFromGit(*gitDoor);
        _objects.push_back(move(door));
    }
}

} // namespace game

} // namespace reone
