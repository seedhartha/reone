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

#include "../game.h"

using namespace std;

using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kMinRotationSpeed = 1.0f;
static constexpr float kMaxRotationSpeed = 2.5f;
static constexpr float kRotationAcceleration = 1.0f;
static constexpr float kMouseRotationSpeed = 0.001f;

ThirdPersonCamera::ThirdPersonCamera(Game *game, SceneGraph *sceneGraph, float aspect, const CameraStyle &style, float zNear, float zFar) : _game(game) {
    glm::mat4 projection(glm::perspective(glm::radians(style.viewAngle), aspect, zNear, zFar));
    _sceneNode = make_unique<CameraSceneNode>(sceneGraph, projection, aspect, zNear, zFar);
    _style = style;
}

bool ThirdPersonCamera::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            return handleKeyDown(event.key);
        case SDL_KEYUP:
            return handleKeyUp(event.key);
        case SDL_MOUSEBUTTONDOWN:
            return handleMouseButtonDown(event.button);
        case SDL_MOUSEBUTTONUP:
            return handleMouseButtonUp(event.button);
        case SDL_MOUSEMOTION:
            return handleMouseMotion(event.motion);
        default:
            return false;
    }
}

bool ThirdPersonCamera::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_A:
            if (!event.repeat && !_mouseLookMode) {
                _rotateCCW = true;
                _rotateCW = false;
                _rotationSpeed = kMinRotationSpeed;
                return true;
            }
            break;
        case SDL_SCANCODE_D:
            if (!event.repeat && !_mouseLookMode) {
                _rotateCCW = false;
                _rotateCW = true;
                _rotationSpeed = kMinRotationSpeed;
                return true;
            }
            break;
        default:
            break;
    }

    return false;
}

bool ThirdPersonCamera::handleKeyUp(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_A:
            if (!_mouseLookMode) {
                _rotateCCW = false;
                return true;
            }
            break;
        case SDL_SCANCODE_D:
            if (!_mouseLookMode) {
                _rotateCW = false;
                return true;
            }
            break;
        default:
            break;
    }

    return false;
}

bool ThirdPersonCamera::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    if (_mouseLookMode) {
        _facing -= kMouseRotationSpeed * event.xrel;
        _facing = glm::mod(_facing, glm::two_pi<float>());
        updateSceneNode();
    }
    return false;
}

bool ThirdPersonCamera::handleMouseButtonDown(const SDL_MouseButtonEvent &event) {
    if (event.button == SDL_BUTTON_RIGHT) {
        _mouseLookMode = true;
        _rotateCCW = false;
        _rotateCW = false;
        _game->setRelativeMouseMode(true);
        return true;
    }
    return false;
}

bool ThirdPersonCamera::handleMouseButtonUp(const SDL_MouseButtonEvent &event) {
    if (event.button == SDL_BUTTON_RIGHT) {
        _mouseLookMode = false;
        _game->setRelativeMouseMode(false);
        return true;
    }
    return false;
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
    glm::vec3 cameraPosition(_targetPosition);
    cameraPosition.x += _style.distance * glm::sin(_facing);
    cameraPosition.y -= _style.distance * glm::cos(_facing);
    cameraPosition.z += _style.height;

    if (_findObstacle) {
        glm::vec3 intersection(0.0f);
        if (_findObstacle(_targetPosition, cameraPosition, intersection)) {
            cameraPosition = intersection;
        }
    }
    glm::vec3 up(0.0f, 0.0f, 1.0f);
    glm::quat orientation(glm::quatLookAt(glm::normalize(_targetPosition - cameraPosition), up));

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, cameraPosition);
    transform *= glm::mat4_cast(orientation);

    _sceneNode->setLocalTransform(transform);
}

void ThirdPersonCamera::stopMovement() {
    _rotateCCW = false;
    _rotateCW = false;
    _mouseLookMode = false;
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
