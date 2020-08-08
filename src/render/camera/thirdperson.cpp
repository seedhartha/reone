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

#include "thirdperson.h"

#include "glm/ext.hpp"

using namespace std;

namespace reone {

namespace render {

static const float kMinRotationSpeed = 1.0f;
static const float kMaxRotationSpeed = 2.5f;
static const float kRotationAcceleration = 1.0f;

ThirdPersonCamera::ThirdPersonCamera(float aspect, const CameraStyle &style, float zNear, float zFar) {
    _projection = glm::perspective(glm::radians(style.viewAngle), aspect, zNear, zFar);
    _style = style;
}

bool ThirdPersonCamera::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            return handleKeyDown(event.key);
        case SDL_KEYUP:
            return handleKeyUp(event.key);
        default:
            return false;
    }
}

bool ThirdPersonCamera::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_A:
            if (!event.repeat) {
                _rotateCCW = true;
                _rotateCW = false;
                _rotationSpeed = kMinRotationSpeed;
            }
            return true;

        case SDL_SCANCODE_D:
            if (!event.repeat) {
                _rotateCCW = false;
                _rotateCW = true;
                _rotationSpeed = kMinRotationSpeed;
            }
            return true;

        default:
            return false;
    }
}

bool ThirdPersonCamera::handleKeyUp(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_A:
            _rotateCCW = false;
            return true;

        case SDL_SCANCODE_D:
            _rotateCW = false;
            return true;

        default:
            return false;
    }
}

void ThirdPersonCamera::update(float dt) {
    if (!_rotateCW && !_rotateCCW) return;

    _rotationSpeed += kRotationAcceleration * dt;
    if (_rotationSpeed > kMaxRotationSpeed) {
        _rotationSpeed = kMaxRotationSpeed;
    }
    _heading += (_rotateCCW ? 1.0f : -1.0f) * _rotationSpeed * dt;
    _heading = glm::mod(_heading, glm::two_pi<float>());

    updateView();
}

void ThirdPersonCamera::updateView() {
    _position = _targetPosition;
    _position.x += _style.distance * glm::sin(_heading);
    _position.y -= _style.distance * glm::cos(_heading);
    _position.z += _style.height;

    if (_findObstacleFunc) {
        glm::vec3 intersection { 0.0f };
        if (_findObstacleFunc(_targetPosition, _position, intersection)) {
            _position = intersection;
        }
    }
    glm::quat orientation(glm::quatLookAt(glm::normalize(_targetPosition - _position), glm::vec3(0.0f, 0.0f, 1.0f)));

    _view = glm::translate(glm::mat4(1.0f), _position);
    _view *= glm::mat4_cast(orientation);

    _view = glm::inverse(_view);
}

void ThirdPersonCamera::resetInput() {
    _rotateCCW = false;
    _rotateCW = false;
}

void ThirdPersonCamera::setTargetPosition(const glm::vec3 &position) {
    _targetPosition = position;
    updateView();
}

void ThirdPersonCamera::setHeading(float heading) {
    _heading = heading;
    updateView();
}

void ThirdPersonCamera::setFindObstacleFunc(const function<bool(const glm::vec3 &, const glm::vec3 &, glm::vec3 &)> &fn) {
    _findObstacleFunc = fn;
}

} // namespace render

} // namespace reone
