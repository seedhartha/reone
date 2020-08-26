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

#include "scenegraph.h"

#include <algorithm>

#include "glm/gtx/norm.hpp"

#include "../mesh/aabb.h"

using namespace std;

namespace reone {

namespace render {

void SceneGraph::clear() {
    _opaqueModels.clear();
    _transparentModels.clear();
    _aabbNodes.clear();
}

void SceneGraph::add(const shared_ptr<ModelSceneNode> &node) {
    if (node->isTransparent()) {
        _transparentModels.push_back(node);
    } else {
        _opaqueModels.push_back(node);
    }
}

void SceneGraph::add(const shared_ptr<AABBSceneNode> &node) {
    _aabbNodes.push_back(node);
}

void SceneGraph::prepare(const glm::vec3 &cameraPosition) {
    for (auto &node : _transparentModels) {
        node->setDistanceToCamera(glm::distance2(node->origin(), cameraPosition));
    }
    std::sort(_transparentModels.begin(), _transparentModels.end(), [](const shared_ptr<ModelSceneNode> &left, const shared_ptr<ModelSceneNode> &right) {
        return left->distanceToCamera() > right->distanceToCamera();
    });
}

void SceneGraph::render() const {
    for (auto &node : _opaqueModels) {
        node->model()->render(*node->modelNode(), node->transform());
    }
    for (auto &node : _transparentModels) {
        node->model()->render(*node->modelNode(), node->transform());
    }
    AABBMesh &aabb = TheAABBMesh;
    for (auto &node : _aabbNodes) {
        aabb.render(node->aabb(), node->transform());
    }
}

} // namespace render

} // namespace reone
