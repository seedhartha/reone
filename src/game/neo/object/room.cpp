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

#include "room.h"

#include "../../../graphics/models.h"
#include "../../../graphics/services.h"
#include "../../../graphics/walkmeshes.h"
#include "../../../scene/graph.h"
#include "../../../scene/graphs.h"
#include "../../../scene/node/model.h"
#include "../../../scene/services.h"

#include "../../services.h"

using namespace std;

using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Room> Room::Loader::load(const string &name, const glm::vec3 &position) {
    auto &scene = _services.scene.graphs.get(kSceneMain);

    shared_ptr<ModelSceneNode> sceneNode;
    auto model = _services.graphics.models.get(name);
    if (model) {
        sceneNode = scene.newModel(move(model), ModelUsage::Room, nullptr);
    }

    shared_ptr<WalkmeshSceneNode> walkmeshSceneNode;
    auto walkmesh = _services.graphics.walkmeshes.get(name, ResourceType::Wok);
    if (walkmesh) {
        walkmeshSceneNode = scene.newWalkmesh(move(walkmesh));
    }

    auto room = Room::Builder()
        .id(_idSeq.nextObjectId())
        .tag(name)
        .sceneNode(move(sceneNode))
        .walkmesh(move(walkmeshSceneNode))
        .build();

    room->setPosition(position);

    return move(room);
}

} // namespace neo

} // namespace game

} // namespace reone