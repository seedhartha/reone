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

#include "thirdperson.h"

#include "glm/ext.hpp"

using namespace std;

using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kMinRotationSpeed = 1.0f;
static constexpr float kMaxRotationSpeed = 2.5f;
static constexpr float kRotationAcceleration = 1.0f;

ThirdPersonCamera::ThirdPersonCamera(SceneGraph *sceneGraph, float aspect, const CameraStyle &style, float zNear, float zFar) {
    glm::mat4 projection(glm::perspective(glm::radians(style.viewAngle), aspect, zNear, zFar));
    _sceneNode = make_unique<CameraSceneNode>(sceneGraph, projection, zFar);
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
    _facing += (_rotateCCW ? 1.0f : -1.0f) * _rotationSpeed * dt;
    _facing = glm::mod(_facing, glm::two_pi<float>());

    updateSceneNode();
}

void ThirdPersonCamera::updateSceneNode() {
    glm::vec3 position(_targetPosition);
    position.x += _style.distance * glm::sin(_facing);
    position.y -= _style.distance * glm::cos(_facing);
    position.z += _style.height;

    if (_findObstacle) {
        glm::vec3 intersection { 0.0f };
        if (_findObstacle(_targetPosition, position, intersection)) {
            position = intersection;
        }
    }
    glm::vec3 up(0.0f, 0.0f, 1.0f);
    glm::quat orientation(glm::quatLookAt(glm::normalize(_targetPosition - position), up));

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, position);
    transform *= glm::mat4_cast(orientation);

    _sceneNode->setLocalTransform(transform);
}

void ThirdPersonCamera::stopMovement() {
    _rotateCCW = false;
    _rotateCW = false;
}

void ThirdPersonCamera::setTargetPosition(const glm::vec3 &position) {
    _targetPosition = position;
    updateSceneNode();
}

void ThirdPersonCamera::setFacing(float facing) {
    _facing = facing;
    updateSceneNode();
}

void ThirdPersonCamera::setFindObstacle(const function<bool(const glm::vec3 &, const glm::vec3 &, glm::vec3 &)> &fn) {
    _findObstacle = fn;
}

void ThirdPersonCamera::setStyle(const CameraStyle &style) {
    _style = style;
    updateSceneNode();
}

} // namespace game

} // namespace reone
