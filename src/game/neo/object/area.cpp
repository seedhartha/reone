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
#include "../../resource/gffs.h"
#include "../../resource/gffstruct.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Area> Area::Loader::load(const std::string &name) {
    info("Loading area " + name);

    auto are = _gffs.get(name, ResourceType::Are);
    if (!are) {
        throw ValidationException("ARE not found: " + name);
    }

    auto git = _gffs.get(name, ResourceType::Git);
    if (!git) {
        throw ValidationException("GIT not found: " + name);
    }

    auto rooms = vector<shared_ptr<Room>>();
    auto areRooms = are->getList("Rooms");
    for (auto &areRoom : areRooms) {
        auto roomName = areRoom->getString("RoomName");
        rooms.push_back(Room::Builder()
                            .id(_idSeq.nextObjectId())
                            .tag(roomName)
                            .build());
    }

    return Area::Builder()
        .id(_idSeq.nextObjectId())
        .tag(name)
        .rooms(move(rooms))
        .build();
}

} // namespace neo

} // namespace game

} // namespace reone