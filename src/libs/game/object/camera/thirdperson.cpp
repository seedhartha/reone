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

#include "reone/game/object/camera/thirdperson.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/scene/collision.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/scene/node/camera.h"

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kMinRotationSpeed = 1.0f;
static constexpr float kMaxRotationSpeed = 2.5f;
static constexpr float kRotationAcceleration = 1.0f;
static constexpr float kMouseRotationSpeed = 0.001f;
static constexpr float kTargetPadding = 0.05f;

void ThirdPersonCamera::load() {
    auto &scene = _services.scene.graphs.get(_sceneName);
    _sceneNode = scene.newCamera();
    cameraSceneNode()->setPerspectiveProjection(glm::radians(_style.viewAngle), _aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);
}

bool ThirdPersonCamera::handle(const input::Event &event) {
    switch (event.type) {
    case input::EventType::KeyDown:
        return handleKeyDown(event.key);
    case input::EventType::KeyUp:
        return handleKeyUp(event.key);
    case input::EventType::MouseButtonDown:
        return handleMouseButtonDown(event.button);
    case input::EventType::MouseButtonUp:
        return handleMouseButtonUp(event.button);
    case input::EventType::MouseMotion:
        return handleMouseMotion(event.motion);
    default:
        return false;
    }
}

bool ThirdPersonCamera::handleKeyDown(const input::KeyEvent &event) {
    switch (event.code) {
    case input::KeyCode::A:
        if (!event.repeat && !_mouseLookMode) {
            _rotateCCW = true;
            _rotateCW = false;
            _rotationSpeed = kMinRotationSpeed;
            return true;
        }
        break;
    case input::KeyCode::D:
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

bool ThirdPersonCamera::handleKeyUp(const input::KeyEvent &event) {
    switch (event.code) {
    case input::KeyCode::A:
        if (!_mouseLookMode) {
            _rotateCCW = false;
            return true;
        }
        break;
    case input::KeyCode::D:
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

bool ThirdPersonCamera::handleMouseMotion(const input::MouseMotionEvent &event) {
    if (_mouseLookMode) {
        _facing -= kMouseRotationSpeed * event.xrel;
        _facing = glm::mod(_facing, glm::two_pi<float>());
        updateSceneNode();
    }
    return false;
}

bool ThirdPersonCamera::handleMouseButtonDown(const input::MouseButtonEvent &event) {
    if (event.button == input::MouseButton::Right) {
        _mouseLookMode = true;
        _rotateCCW = false;
        _rotateCW = false;
        _game.setRelativeMouseMode(true);
        return true;
    }
    return false;
}

bool ThirdPersonCamera::handleMouseButtonUp(const input::MouseButtonEvent &event) {
    if (event.button == input::MouseButton::Right) {
        _mouseLookMode = false;
        _game.setRelativeMouseMode(false);
        return true;
    }
    return false;
}

void ThirdPersonCamera::update(float dt) {
    if (!_rotateCW && !_rotateCCW)
        return;

    _rotationSpeed += kRotationAcceleration * dt;

    if (_rotationSpeed > kMaxRotationSpeed) {
        _rotationSpeed = kMaxRotationSpeed;
    }
    _facing += (_rotateCCW ? 1.0f : -1.0f) * _rotationSpeed * dt;
    _facing = glm::mod(_facing, glm::two_pi<float>());

    updateSceneNode();
}

void ThirdPersonCamera::updateSceneNode() {
    static glm::vec3 up {0.0f, 0.0f, 1.0f};

    glm::vec3 dir(
        glm::sin(_facing),
        -glm::cos(_facing),
        0.0f);

    glm::vec3 targetPos(_targetPosition);
    targetPos += kTargetPadding * dir;

    glm::vec3 cameraPos(_targetPosition);
    cameraPos += _style.distance * dir;
    cameraPos.z += _style.height;

    Collision collision;
    auto &scene = _services.scene.graphs.get(_sceneName);
    if (scene.testLineOfSight(targetPos, cameraPos, collision)) {
        cameraPos = collision.intersection;
    }

    glm::quat orientation(glm::quatLookAt(glm::normalize(targetPos - cameraPos), up));

    glm::mat4 transform(1.0f);
    transform *= glm::translate(cameraPos);
    transform *= glm::mat4_cast(orientation);
    _sceneNode->setLocalTransform(std::move(transform));
}

void ThirdPersonCamera::stopMovement() {
    _rotateCCW = false;
    _rotateCW = false;
    _mouseLookMode = false;
}

void ThirdPersonCamera::setTargetPosition(glm::vec3 position) {
    _targetPosition = std::move(position);
    updateSceneNode();
}

void ThirdPersonCamera::setFacing(float facing) {
    _facing = facing;
    updateSceneNode();
}

void ThirdPersonCamera::setStyle(CameraStyle style) {
    _style = std::move(style);
    updateSceneNode();
}

} // namespace game

} // namespace reone
