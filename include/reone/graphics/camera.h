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

#pragma once

#include "aabb.h"
#include "types.h"

namespace reone {

namespace graphics {

class Camera {
public:
    Camera(CameraType type) :
        _type(type) {
    }

    virtual ~Camera() = default;

    bool isInFrustum(const glm::vec3 &pt) const {
        glm::vec4 pt4(pt, 1.0f);
        return glm::dot(_frustumLeft, pt4) >= 0.0f &&
               glm::dot(_frustumRight, pt4) >= 0.0f &&
               glm::dot(_frustumBottom, pt4) >= 0.0f &&
               glm::dot(_frustumTop, pt4) >= 0.0f &&
               glm::dot(_frustumNear, pt4) >= 0.0f &&
               glm::dot(_frustumFar, pt4) >= 0.0f;
    }

    bool isInFrustum(const AABB &aabb) const {
        for (auto &corner : aabb.corners()) {
            if (isInFrustum(corner)) {
                return true;
            }
        }
        return false;
    }

    CameraType type() const { return _type; }
    const glm::mat4 &projection() const { return _projection; }
    const glm::mat4 &view() const { return _view; }
    const glm::mat4 &viewInv() const { return _viewInv; }
    const glm::vec3 &position() { return _position; }
    float zNear() const { return _zNear; }
    float zFar() const { return _zFar; }

    void setView(glm::mat4 view) {
        _viewInv = glm::inverse(view);
        _position = _viewInv[3];
        _view = std::move(view);
        updateFrustumPlanes();
    }

protected:
    glm::mat4 _projection {1.0f};
    glm::mat4 _view {1.0f};
    glm::mat4 _viewInv {1.0f};
    glm::vec3 _position {0.0f};
    float _zNear {0.0f};
    float _zFar {0.0f};

    void setProjection(glm::mat4 proj, glm::mat4 frustumProj) {
        _projection = std::move(proj);
        _frustumProjection = std::move(frustumProj);
        updateFrustumPlanes();
    }

private:
    CameraType _type;

    glm::mat4 _frustumProjection {1.0f};

    glm::vec4 _frustumLeft {0.0f};
    glm::vec4 _frustumRight {0.0f};
    glm::vec4 _frustumBottom {0.0f};
    glm::vec4 _frustumTop {0.0f};
    glm::vec4 _frustumNear {0.0f};
    glm::vec4 _frustumFar {0.0f};

    void updateFrustumPlanes() {
        glm::mat4 vp(_frustumProjection * _view);
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
};

} // namespace graphics

} // namespace reone
