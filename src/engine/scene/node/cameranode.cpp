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

#include "cameranode.h"

#include "glm/ext.hpp"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

CameraSceneNode::CameraSceneNode(string name, glm::mat4 projection, SceneGraph *sceneGraph) :
    SceneNode(move(name), SceneNodeType::Camera, sceneGraph),
    _projection(projection) {
}

void CameraSceneNode::onAbsoluteTransformChanged() {
    computeView();
    computeFrustumPlanes();
}

void CameraSceneNode::computeView() {
    _view = _absTransformInv;
}

void CameraSceneNode::computeFrustumPlanes() {
    // Implementation of http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf

    glm::mat4 vp(_projection * _view);
    for (int i = 3; i >= 0; --i) {
        _frustumLeft[i] = vp[i][3] + vp[i][0];
        _frustumRight[i] = vp[i][3] - vp[i][0];
        _frustumBottom[i] = vp[i][3] + vp[i][1];
        _frustumTop[i] = vp[i][3] - vp[i][1];
        _frustumNear[i] = vp[i][3] + vp[i][2];
        _frustumFar[i] = vp[i][3] - vp[i][2];
    }

    _frustumLeft = glm::normalize(_frustumLeft);
    _frustumRight = glm::normalize(_frustumRight);
    _frustumBottom = glm::normalize(_frustumBottom);
    _frustumTop = glm::normalize(_frustumTop);
    _frustumNear = glm::normalize(_frustumNear);
    _frustumFar = glm::normalize(_frustumFar);
}

bool CameraSceneNode::isInFrustum(const glm::vec3 &point) const {
    glm::vec4 point4(point, 1.0f);

    if (glm::dot(_frustumLeft, point4) < 0.0f) return false;
    if (glm::dot(_frustumRight, point4) < 0.0f) return false;
    if (glm::dot(_frustumBottom, point4) < 0.0f) return false;
    if (glm::dot(_frustumTop, point4) < 0.0f) return false;
    if (glm::dot(_frustumNear, point4) < 0.0f) return false;
    if (glm::dot(_frustumFar, point4) < 0.0f) return false;

    return true;
}

bool CameraSceneNode::isInFrustum(const AABB &aabb) const {
    // AABB is inside frustum if at least one of its corners is inside

    glm::vec3 center(aabb.center());
    glm::vec3 halfSize(aabb.getSize() * 0.5f);

    vector<glm::vec3> corners {
        glm::vec3(center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z),
        glm::vec3(center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z),
        glm::vec3(center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z),
        glm::vec3(center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z),
        glm::vec3(center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z),
        glm::vec3(center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z),
        glm::vec3(center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z),
        glm::vec3(center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z)
    };

    for (auto &corner : corners) {
        if (isInFrustum(corner)) return true;
    }

    return false;
}

bool CameraSceneNode::isInFrustum(const SceneNode &other) const {
    return other.isVolumetric() ?
        isInFrustum(other.aabb() * other.absoluteTransform()) :
        isInFrustum(other.absoluteTransform()[3]);
}

void CameraSceneNode::setProjection(glm::mat4 projection) {
    _projection = move(projection);
    computeFrustumPlanes();
}

} // namespace scene

} // namespace reone
