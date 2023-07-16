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

#include "reone/game/object/camera/firstperson.h"

#include "reone/game/di/services.h"
#include "reone/graphics/types.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/scene/node/camera.h"

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kMovementSpeed = 4.0f;
static constexpr float kMouseMultiplier = glm::pi<float>() / 4000.0f;

void FirstPersonCamera::load() {
    auto &scene = _services.scene.graphs.get(_sceneName);
    _sceneNode = scene.newCamera();
    cameraSceneNode()->setPerspectiveProjection(_fovy, _aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);
}

bool FirstPersonCamera::handle(const SDL_Event &event) {
    switch (event.type) {
    case SDL_MOUSEMOTION:
        return handleMouseMotion(event.motion);
    case SDL_KEYDOWN:
        return handleKeyDown(event.key);
    case SDL_KEYUP:
        return handleKeyUp(event.key);
    default:
        return false;
    }
}

bool FirstPersonCamera::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    _facing = glm::mod(
        _facing - event.xrel * kMouseMultiplier,
        glm::two_pi<float>());

    _pitch = glm::clamp(
        _pitch - event.yrel * kMouseMultiplier,
        -glm::quarter_pi<float>(),
        glm::quarter_pi<float>());

    updateSceneNode();

    return true;
}

void FirstPersonCamera::updateSceneNode() {
    glm::quat orientation(glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f));
    orientation *= glm::quat(glm::vec3(_pitch, _facing, 0.0f));

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, _position);
    transform *= glm::mat4_cast(orientation);

    _sceneNode->setLocalTransform(transform);
}

bool FirstPersonCamera::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
    case SDL_SCANCODE_W:
        _moveForward = true;
        return true;

    case SDL_SCANCODE_A:
        _moveLeft = true;
        return true;

    case SDL_SCANCODE_S:
        _moveBackward = true;
        return true;

    case SDL_SCANCODE_D:
        _moveRight = true;
        return true;

    case SDL_SCANCODE_LSHIFT:
        _multiplier = 2.0f;
        return true;

    default:
        return false;
    }
}

bool FirstPersonCamera::handleKeyUp(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
    case SDL_SCANCODE_W:
        _moveForward = false;
        return true;

    case SDL_SCANCODE_A:
        _moveLeft = false;
        return true;

    case SDL_SCANCODE_S:
        _moveBackward = false;
        return true;

    case SDL_SCANCODE_D:
        _moveRight = false;
        return true;

    case SDL_SCANCODE_LSHIFT:
        _multiplier = 1.0f;
        return true;

    default:
        return false;
    }
}

void FirstPersonCamera::update(float dt) {
    float facingSin = glm::sin(_facing) * _multiplier * kMovementSpeed * dt;
    float facingCos = glm::cos(_facing) * _multiplier * kMovementSpeed * dt;
    float pitchSin = glm::sin(_pitch) * _multiplier * kMovementSpeed * dt;
    bool positionChanged = false;

    if (_moveForward) {
        _position.x -= facingSin;
        _position.y += facingCos;
        _position.z += pitchSin;
        positionChanged = true;
    }
    if (_moveLeft) {
        _position.x -= facingCos;
        _position.y -= facingSin;
        positionChanged = true;
    }
    if (_moveBackward) {
        _position.x += facingSin;
        _position.y -= facingCos;
        _position.z -= pitchSin;
        positionChanged = true;
    }
    if (_moveRight) {
        _position.x += facingCos;
        _position.y += facingSin;
        positionChanged = true;
    }
    if (positionChanged) {
        updateSceneNode();
    }
}

void FirstPersonCamera::stopMovement() {
    _moveForward = false;
    _moveLeft = false;
    _moveBackward = false;
    _moveRight = false;
}

void FirstPersonCamera::setPosition(const glm::vec3 &pos) {
    _position = pos;
    updateSceneNode();
}

void FirstPersonCamera::setFacing(float facing) {
    _facing = facing;
    updateSceneNode();
}

} // namespace game

} // namespace reone
