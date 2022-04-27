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
#include "../../../resource/gffstruct.h"
#include "../../../resource/services.h"

#include "../../layouts.h"
#include "../../services.h"

#include "creature.h"
#include "door.h"
#include "placeable.h"

using namespace std;

using namespace reone::scene;
using namespace reone::resource;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Area> Area::Loader::load(const std::string &name) {
    info("Loading area " + name);

    auto are = _services.resource.gffs.get(name, ResourceType::Are);
    if (!are) {
        throw ValidationException("ARE not found: " + name);
    }

    auto git = _services.resource.gffs.get(name, ResourceType::Git);
    if (!git) {
        throw ValidationException("GIT not found: " + name);
    }

    auto layout = _services.game.layouts.get(name);
    if (!layout) {
        throw ValidationException("LYT not found: " + name);
    }

    // Rooms
    auto rooms = vector<shared_ptr<Room>>();
    auto roomLoader = Room::Loader(_idSeq, _services);
    auto areRooms = are->getList("Rooms");
    for (auto &areRoom : areRooms) {
        auto roomName = areRoom->getString("RoomName");
        auto layoutRoom = layout->findByName(roomName);
        rooms.push_back(roomLoader.load(roomName, layoutRoom->position));
    }

    auto area = Area::Builder()
                    .id(_idSeq.nextObjectId())
                    .tag(name)
                    .rooms(move(rooms))
                    .build();

    // Creatures
    auto creatureLoader = Creature::Loader(_idSeq, _services);
    auto gitCreatures = git->getList("Creature List");
    for (auto &gitCreature : gitCreatures) {
        area->add(creatureLoader.load(*gitCreature));
    }

    // Placeables
    auto placeableLoader = Placeable::Loader(_idSeq, _services);
    auto gitPlaceables = git->getList("Placeable List");
    for (auto &gitPlaceable : gitPlaceables) {
        area->add(placeableLoader.load(*gitPlaceable));
    }

    // Doors
    auto doorLoader = Door::Loader(_idSeq, _services);
    auto gitDoors = git->getList("Door List");
    for (auto &gitDoor : gitDoors) {
        area->add(doorLoader.load(*gitDoor));
    }

    return move(area);
}

} // namespace neo

} // namespace game

} // namespace reone