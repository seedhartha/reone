/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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
        _frustum.left[i] = vp[i][3] + vp[i][0];
        _frustum.right[i] = vp[i][3] - vp[i][0];
        _frustum.bottom[i] = vp[i][3] + vp[i][1];
        _frustum.top[i] = vp[i][3] - vp[i][1];
        _frustum.near[i] = vp[i][3] + vp[i][2];
        _frustum.far[i] = vp[i][3] - vp[i][2];
    }
    _frustum.left = glm::normalize(_frustum.left);
    _frustum.right = glm::normalize(_frustum.right);
    _frustum.bottom = glm::normalize(_frustum.bottom);
    _frustum.top = glm::normalize(_frustum.top);
    _frustum.near = glm::normalize(_frustum.near);
    _frustum.far = glm::normalize(_frustum.far);
}

bool CameraSceneNode::isInFrustum(const glm::vec3 &point) const {
    glm::vec4 point4(point, 1.0f);
    bool result =
        glm::dot(_frustum.left, point4) >= 0.0f &&
        glm::dot(_frustum.right, point4) >= 0.0f &&
        glm::dot(_frustum.bottom, point4) >= 0.0f &&
        glm::dot(_frustum.top, point4) >= 0.0f &&
        glm::dot(_frustum.near, point4) >= 0.0f &&
        glm::dot(_frustum.far, point4) >= 0.0f;

    return result;
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
