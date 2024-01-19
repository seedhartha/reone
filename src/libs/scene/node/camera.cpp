/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/scene/node/camera.h"

#include "reone/graphics/camera/orthographic.h"
#include "reone/graphics/camera/perspective.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

void CameraSceneNode::onAbsoluteTransformChanged() {
    if (_camera) {
        _camera->setView(_absTransformInv);
    }
}

bool CameraSceneNode::isInFrustum(const SceneNode &other) const {
    if (!_camera) {
        return false;
    }
    if (other.isPoint()) {
        return _camera->isInFrustum(other.getOrigin());
    } else {
        return _camera->isInFrustum(other.aabb() * other.absoluteTransform());
    }
}

void CameraSceneNode::setOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar) {
    auto camera = std::make_unique<OrthographicCamera>();
    camera->setProjection(left, right, bottom, top, zNear, zFar);
    camera->setView(_absTransformInv);
    _camera = std::move(camera);
}

void CameraSceneNode::setPerspectiveProjection(float fovy, float aspect, float zNear, float zFar) {
    auto camera = std::make_shared<PerspectiveCamera>();
    camera->setProjection(fovy, aspect, zNear, zFar);
    camera->setView(_absTransformInv);
    _camera = std::move(camera);
}

} // namespace scene

} // namespace reone
