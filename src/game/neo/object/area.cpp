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
#include "../../../common/streamutil.h"
#include "../../../graphics/models.h"
#include "../../../graphics/services.h"
#include "../../../resource/gffs.h"
#include "../../../resource/gffstruct.h"
#include "../../../resource/services.h"
#include "../../../scene/graph.h"
#include "../../../scene/graphs.h"
#include "../../../scene/node/model.h"
#include "../../../scene/services.h"

#include "../../layouts.h"
#include "../../services.h"

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

    auto rooms = vector<shared_ptr<Room>>();
    auto areRooms = are->getList("Rooms");
    for (auto &areRoom : areRooms) {
        auto roomName = areRoom->getString("RoomName");

        auto lytRoom = layout->findByName(roomName);
        auto transform = glm::translate(lytRoom->position);

        auto &scene = _services.scene.sceneGraphs.get(kSceneMain);
        auto model = _services.graphics.models.get(roomName);
        auto sceneNode = scene.newModel(move(model), ModelUsage::Room, nullptr);
        sceneNode->setLocalTransform(move(transform));

        rooms.push_back(Room::Builder()
                            .id(_idSeq.nextObjectId())
                            .tag(move(roomName))
                            .sceneNode(move(sceneNode))
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