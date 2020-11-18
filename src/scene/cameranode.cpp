/*
 * Copyright (c) 2020 The reone project contributors
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

using namespace reone::render;

namespace reone {

namespace scene {

CameraSceneNode::CameraSceneNode(SceneGraph *sceneGraph, const glm::mat4 &projection) :
    SceneNode(sceneGraph), _projection(projection) {

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
        _frustum[0][i] = vp[i][3] + vp[i][0];
        _frustum[1][i] = vp[i][3] - vp[i][0];
        _frustum[2][i] = vp[i][3] + vp[i][1];
        _frustum[3][i] = vp[i][3] - vp[i][1];
        _frustum[4][i] = vp[i][3] + vp[i][2];
        _frustum[5][i] = vp[i][3] - vp[i][2];
    }
    for (int i = 0; i < kFrustumPlaneCount; ++i) {
        _frustum[i] = glm::normalize(_frustum[i]);
    }
}

bool CameraSceneNode::isInFrustum(const glm::vec3 &point) const {
    glm::vec4 point4(point, 1.0f);
    for (int i = 0; i < kFrustumPlaneCount; ++i) {
        if (glm::dot(_frustum[i], point4) < 0.0f) return false;
    }
    return true;
}

bool CameraSceneNode::isInFrustum(const AABB &aabb) const {
    glm::vec3 center(aabb.center());
    glm::vec3 halfSize(aabb.size() * 0.5f);

    for (int i = 0; i < kFrustumPlaneCount; ++i) {
        if (glm::dot(_frustum[i], glm::vec4(center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustum[i], glm::vec4(center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustum[i], glm::vec4(center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustum[i], glm::vec4(center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustum[i], glm::vec4(center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustum[i], glm::vec4(center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustum[i], glm::vec4(center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z, 1.0f)) >= 0.0f) continue;
        if (glm::dot(_frustum[i], glm::vec4(center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z, 1.0f)) >= 0.0f) continue;

        return false;
    }

    return true;
}

const glm::mat4 &CameraSceneNode::projection() const {
    return _projection;
}

const glm::mat4 &CameraSceneNode::view() const {
    return _view;
}

void CameraSceneNode::setProjection(const glm::mat4 &projection) {
    _projection = projection;
    updateFrustum();
}

} // namespace scene

} // namespace reone
