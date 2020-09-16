/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "glm/gtx/norm.hpp"

#include "../render/mesh/aabb.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::render;

namespace reone {

namespace game {

void Area::updateSceneGraph(const glm::vec3 &cameraPosition) {
    _sceneGraph.clear();

    for (auto &room : _rooms) {
        shared_ptr<ModelSceneNode> model(room->model());
        if (!model) continue;

        glm::mat4 transform(glm::translate(glm::mat4(1.0f), room->position()));
        model->fill(_sceneGraph, transform, _debugMode == DebugMode::ModelNodes);
    }
    for (auto &object : _objects) {
        shared_ptr<ModelSceneNode> model(object->model());
        if (!model) continue;

        model->fill(_sceneGraph, object->transform(), _debugMode == DebugMode::ModelNodes);
    }
    switch (_debugMode) {
        case DebugMode::GameObjects:
            for (auto &list : _objectsByType) {
                ObjectType type = list.first;
                if (type != ObjectType::Creature && type != ObjectType::Placeable && type != ObjectType::Door) continue;

                for (auto &object : list.second) {
                    shared_ptr<ModelSceneNode> model(object->model());
                    if (!model) continue;

                    shared_ptr<AABBSceneNode> node(new AABBSceneNode(model->model()->aabb(), object->transform()));
                    _sceneGraph.add(node);
                }
            }
            break;
        default:
            break;
    }

    _sceneGraph.prepare(cameraPosition);
}

void Area::render() const {
    _sceneGraph.render();
}

} // namespace game

} // namespace reone
