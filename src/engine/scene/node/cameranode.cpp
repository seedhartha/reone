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

using namespace reone::graphics;

namespace reone {

namespace scene {

CameraSceneNode::CameraSceneNode(SceneGraph *sceneGraph, glm::mat4 projection, float aspect, float nearPlane, float farPlane) :
    SceneNode(SceneNodeType::Camera, sceneGraph),
    _projection(projection),
    _aspect(aspect),
    _nearPlane(nearPlane),
    _farPlane(farPlane) {

    updateFrustum();
}

void CameraSceneNode::updateAbsoluteTransform() {
    SceneNode::updateAbsoluteTransform();
    updateView();
    updateFrustum();
}

void CameraSceneNode::updateView() {
    _view = glm::inverse(_absoluteTransform);
}

void CameraSceneNode::updateFrustum() {
    glm::mat4 vp(_projection * _view);
    for (int i = 3; i >= 0; --i) {
        _frustumPlanes[0][i] = vp[i][3] + vp[i][0];
        _frustumPlanes[1][i] = vp[i][3] - vp[i][0];
        _frustumPlanes[2][i] = vp[i][3] + vp[i][1];
        _frustumPlanes[3][i] = vp[i][3] - vp[i][1];
        _frustumPlanes[4][i] = vp[i][3] + vp[i][2];
        _frustumPlanes[5][i] = vp[i][3] - vp[i][2];
    }
    for (int i = 0; i < kNumFrustumPlanes; ++i) {
        _frustumPlanes[i] = glm::normalize(_frustumPlanes[i]);
    }
}

bool CameraSceneNode::isInFrustum(const glm::vec3 &point) const {
    glm::vec4 point4(point, 1.0f);
    for (int i = 0; i < kNumFrustumPlanes; ++i) {
        if (glm::dot(_frustumPlanes[i], point4) < 0.0f) return false;
    }
    return true;
}

bool CameraSceneNode::isInFrustum(const AABB &aabb) const {
    glm::vec3 center(aabb.center());
    glm::vec3 halfSize(aabb.getSize() * 0.5f);

    for (int i = 0; i < kNumFrustumPlanes; ++i) {
        if (glm::dot(_frustumPlanes[i], glm::vec4(center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustumPlanes[i], glm::vec4(center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustumPlanes[i], glm::vec4(center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustumPlanes[i], glm::vec4(center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustumPlanes[i], glm::vec4(center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustumPlanes[i], glm::vec4(center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustumPlanes[i], glm::vec4(center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustumPlanes[i], glm::vec4(center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z, 1.0f)) >= 0.0f) continue;

        return false;
    }

    return true;
}

void CameraSceneNode::setProjection(const glm::mat4 &projection) {
    _projection = projection;
    updateFrustum();
}

} // namespace scene

} // namespace reone
