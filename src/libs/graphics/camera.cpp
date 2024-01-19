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

#include "reone/graphics/camera.h"

namespace reone {

namespace graphics {

void Camera::updateFrustum() {
    auto vp = _projection * _view;
    for (int i = 0; i < 3; ++i) {
        _frustum.planes[0].normal[i] = vp[i][3] + vp[i][0];
        _frustum.planes[1].normal[i] = vp[i][3] - vp[i][0];
        _frustum.planes[2].normal[i] = vp[i][3] + vp[i][1];
        _frustum.planes[3].normal[i] = vp[i][3] - vp[i][1];
        _frustum.planes[4].normal[i] = vp[i][3] + vp[i][2];
        _frustum.planes[5].normal[i] = vp[i][3] - vp[i][2];
    }
    _frustum.planes[0].distance = vp[3][3] + vp[3][0];
    _frustum.planes[1].distance = vp[3][3] - vp[3][0];
    _frustum.planes[2].distance = vp[3][3] + vp[3][1];
    _frustum.planes[3].distance = vp[3][3] - vp[3][1];
    _frustum.planes[4].distance = vp[3][3] + vp[3][2];
    _frustum.planes[5].distance = vp[3][3] - vp[3][2];
    for (auto &plane : _frustum.planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }
}

bool Camera::isInFrustum(const glm::vec3 &point) const {
    for (const auto &plane : _frustum.planes) {
        if (plane.distanceTo(point) < 0.0f) {
            return false;
        }
    }
    return true;
}

bool Camera::isInFrustum(const AABB &aabb) const {
    for (const auto &plane : _frustum.planes) {
        auto codir = aabb.max();
        auto contradir = aabb.min();
        if (plane.normal.x < 0.0) {
            codir.x = aabb.min().x;
            contradir.x = aabb.max().x;
        }
        if (plane.normal.y < 0.0) {
            codir.y = aabb.min().y;
            contradir.y = aabb.max().y;
        }
        if (plane.normal.z < 0.0) {
            codir.z = aabb.min().z;
            contradir.z = aabb.max().z;
        }
        if (plane.distanceTo(codir) < 0.0f) {
            return false;
        }
    }
    return true;
}

} // namespace graphics

} // namespace reone
