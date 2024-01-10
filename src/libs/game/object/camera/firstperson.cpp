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

bool FirstPersonCamera::handle(const input::Event &event) {
    switch (event.type) {
    case input::EventType::MouseMotion:
        return handleMouseMotion(event.motion);
    case input::EventType::KeyDown:
        return handleKeyDown(event.key);
    case input::EventType::KeyUp:
        return handleKeyUp(event.key);
    default:
        return false;
    }
}

bool FirstPersonCamera::handleMouseMotion(const input::MouseMotionEvent &event) {
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

bool FirstPersonCamera::handleKeyDown(const input::KeyEvent &event) {
    switch (event.code) {
    case input::KeyCode::D:
        _moveDir = MovementDirection::Right;
        return true;

    case input::KeyCode::A:
        _moveDir = MovementDirection::Left;
        return true;

    case input::KeyCode::W:
        _moveDir = MovementDirection::Forward;
        return true;

    case input::KeyCode::S:
        _moveDir = MovementDirection::Back;
        return true;

    case input::KeyCode::Q:
        _moveDir = MovementDirection::Up;
        return true;

    case input::KeyCode::Z:
        _moveDir = MovementDirection::Down;
        return true;

    case input::KeyCode::LeftShift:
        _multiplier = 2.0f;
        return true;

    default:
        return false;
    }
}

bool FirstPersonCamera::handleKeyUp(const input::KeyEvent &event) {
    switch (event.code) {
    case input::KeyCode::D:
        if (_moveDir == MovementDirection::Right) {
            _moveDir = MovementDirection::None;
        }
        return true;

    case input::KeyCode::A:
        if (_moveDir == MovementDirection::Left) {
            _moveDir = MovementDirection::None;
        }
        return true;

    case input::KeyCode::W:
        if (_moveDir == MovementDirection::Forward) {
            _moveDir = MovementDirection::None;
        }
        return true;

    case input::KeyCode::S:
        if (_moveDir == MovementDirection::Back) {
            _moveDir = MovementDirection::None;
        }
        return true;

    case input::KeyCode::Q:
        if (_moveDir == MovementDirection::Up) {
            _moveDir = MovementDirection::None;
        }
        return true;

    case input::KeyCode::Z:
        if (_moveDir == MovementDirection::Down) {
            _moveDir = MovementDirection::None;
        }
        return true;

    case input::KeyCode::LeftShift:
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
    float pitchCos = glm::cos(_pitch) * _multiplier * kMovementSpeed * dt;
    bool positionChanged = false;

    switch (_moveDir) {
    case MovementDirection::Right:
        _position.x += facingCos;
        _position.y += facingSin;
        positionChanged = true;
        break;
    case MovementDirection::Left:
        _position.x -= facingCos;
        _position.y -= facingSin;
        positionChanged = true;
        break;
    case MovementDirection::Forward:
        _position.x -= facingSin;
        _position.y += facingCos;
        _position.z += pitchSin;
        positionChanged = true;
        break;
    case MovementDirection::Back:
        _position.x += facingSin;
        _position.y -= facingCos;
        _position.z -= pitchSin;
        positionChanged = true;
        break;
    case MovementDirection::Up:
        _position.x += facingSin * pitchSin;
        _position.y -= facingCos * pitchSin;
        _position.z += pitchCos;
        positionChanged = true;
        break;
    case MovementDirection::Down:
        _position.x -= facingSin * pitchSin;
        _position.y += facingCos * pitchSin;
        _position.z -= pitchCos;
        positionChanged = true;
        break;
    default:
        break;
    }
    if (positionChanged) {
        updateSceneNode();
    }
}

void FirstPersonCamera::stopMovement() {
    _moveDir = MovementDirection::None;
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
