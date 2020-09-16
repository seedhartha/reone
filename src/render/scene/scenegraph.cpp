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
#include "../modelinstance.h"

using namespace std;

namespace reone {

namespace render {

void SceneGraph::clear() {
    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _aabbNodes.clear();
}

void SceneGraph::add(const shared_ptr<MeshSceneNode> &node) {
    if (node->isTransparent()) {
        _transparentMeshes.push_back(node);
    } else {
        _opaqueMeshes.push_back(node);
    }
}

void SceneGraph::add(const shared_ptr<AABBSceneNode> &node) {
    _aabbNodes.push_back(node);
}

void SceneGraph::prepare(const glm::vec3 &cameraPosition) {
    for (auto &node : _transparentMeshes) {
        node->setDistanceToCamera(glm::distance2(node->origin(), cameraPosition));
    }
    std::sort(_transparentMeshes.begin(), _transparentMeshes.end(), [](const shared_ptr<MeshSceneNode> &left, const shared_ptr<MeshSceneNode> &right) {
        return left->distanceToCamera() > right->distanceToCamera();
    });
}

void SceneGraph::render() const {
    for (auto &node : _opaqueMeshes) {
        node->model()->render(*node->modelNode(), node->transform());
    }
    for (auto &node : _transparentMeshes) {
        node->model()->render(*node->modelNode(), node->transform());
    }
    AABBMesh &aabb = TheAABBMesh;
    for (auto &node : _aabbNodes) {
        aabb.render(node->aabb(), node->transform());
    }
}

} // namespace render

} // namespace reone
