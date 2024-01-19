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

#include "../camera.h"

namespace reone {

namespace graphics {

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera() :
        Camera(CameraType::Perspective) {
    }

    float fovy() const { return _fovy; }
    float aspect() const { return _aspect; }

    void setProjection(float fovy, float aspect, float zNear, float zFar) {
        _fovy = fovy;
        _aspect = aspect;
        _zNear = zNear;
        _zFar = zFar;

        Camera::setProjection(glm::perspective(fovy, aspect, zNear, zFar));
    }

private:
    float _fovy {0.0f};
    float _aspect {1.0f};
};

} // namespace graphics

} // namespace reone
