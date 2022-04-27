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
#include "../../../scene/graph.h"
#include "../../../scene/graphs.h"
#include "../../../scene/node/model.h"
#include "../../../scene/services.h"

#include "../../services.h"

using namespace std;

using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Room> Room::Loader::load(const string &name, const glm::vec3 &position) {
    auto &scene = _services.scene.graphs.get(kSceneMain);
    auto model = _services.graphics.models.get(name);
    auto sceneNode = scene.newModel(move(model), ModelUsage::Room, nullptr);

    auto transform = glm::translate(position);
    sceneNode->setLocalTransform(move(transform));

    return Room::Builder()
        .id(_idSeq.nextObjectId())
        .tag(name)
        .sceneNode(move(sceneNode))
        .build();
}

} // namespace neo

} // namespace game

} // namespace reone