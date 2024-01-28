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

#include "reone/game/neo/controller/playercamera.h"

#include "reone/game/neo/action.h"
#include "reone/game/neo/object/creature.h"
#include "reone/input/event.h"
#include "reone/scene/collision.h"
#include "reone/scene/graph.h"

using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

static constexpr float kCameraDistance = 3.2f;
static constexpr float kCameraHeight = 1.6f + 0.4f;

static constexpr float kCameraMouseSensitity = 0.001f;
static constexpr float kCameraMoveRate = 8.0f;
static constexpr float kCameraTurnRate = 4.0f;

bool PlayerCameraController::handle(const input::Event &event) {
    if (_player) {
        switch (event.type) {
        case input::EventType::KeyDown:
            switch (event.key.code) {
            case input::KeyCode::W:
                _commandMask |= CommandTypes::MovePlayerFront;
                return true;
            case input::KeyCode::A:
                _commandMask |= CommandTypes::RotateCameraCCW;
                return true;
            case input::KeyCode::S:
                _commandMask |= CommandTypes::MovePlayerBack;
                return true;
            case input::KeyCode::D:
                _commandMask |= CommandTypes::RotateCameraCW;
                return true;
            case input::KeyCode::Z:
                _commandMask |= CommandTypes::MovePlayerLeft;
                return true;
            case input::KeyCode::C:
                _commandMask |= CommandTypes::MovePlayerRight;
                return true;
            default:
                break;
            }
            break;
        case input::EventType::KeyUp:
            switch (event.key.code) {
            case input::KeyCode::W:
                _commandMask &= ~CommandTypes::MovePlayerFront;
                return true;
            case input::KeyCode::A:
                _commandMask &= ~CommandTypes::RotateCameraCCW;
                return true;
            case input::KeyCode::S:
                _commandMask &= ~CommandTypes::MovePlayerBack;
                return true;
            case input::KeyCode::D:
                _commandMask &= ~CommandTypes::RotateCameraCW;
                return true;
            case input::KeyCode::Z:
                _commandMask &= ~CommandTypes::MovePlayerLeft;
                return true;
            case input::KeyCode::C:
                _commandMask &= ~CommandTypes::MovePlayerRight;
                return true;
            default:
                break;
            }
            break;
        default:
            break;
        }
    } else {
        switch (event.type) {
        case input::EventType::MouseMotion:
            _cameraPitch += -kCameraMouseSensitity * event.motion.yrel;
            _cameraFacing += -kCameraMouseSensitity * event.motion.xrel;
            return true;
        case input::EventType::KeyDown:
            switch (event.key.code) {
            case input::KeyCode::W:
                _commandMask |= CommandTypes::MoveCameraFront;
                return true;
            case input::KeyCode::A:
                _commandMask |= CommandTypes::MoveCameraLeft;
                return true;
            case input::KeyCode::S:
                _commandMask |= CommandTypes::MoveCameraBack;
                return true;
            case input::KeyCode::D:
                _commandMask |= CommandTypes::MoveCameraRight;
                return true;
            case input::KeyCode::Q:
                _commandMask |= CommandTypes::MoveCameraUp;
                return true;
            case input::KeyCode::Z:
                _commandMask |= CommandTypes::MoveCameraDown;
                return true;
            default:
                break;
            }
            break;
        case input::EventType::KeyUp:
            switch (event.key.code) {
            case input::KeyCode::W:
                _commandMask &= ~CommandTypes::MoveCameraFront;
                return true;
            case input::KeyCode::A:
                _commandMask &= ~CommandTypes::MoveCameraLeft;
                return true;
            case input::KeyCode::S:
                _commandMask &= ~CommandTypes::MoveCameraBack;
                return true;
            case input::KeyCode::D:
                _commandMask &= ~CommandTypes::MoveCameraRight;
                return true;
            case input::KeyCode::Q:
                _commandMask &= ~CommandTypes::MoveCameraUp;
                return true;
            case input::KeyCode::Z:
                _commandMask &= ~CommandTypes::MoveCameraDown;
                return true;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    return false;
}

void PlayerCameraController::update(float dt) {
    if (_player) {
        if (_commandMask & CommandTypes::RotateCameraCCW) {
            _cameraFacing += kCameraTurnRate * dt;
        } else if (_commandMask & CommandTypes::RotateCameraCW) {
            _cameraFacing -= kCameraTurnRate * dt;
        }
        float sinFacing = glm::sin(_cameraFacing);
        float cosFacing = glm::cos(_cameraFacing);
        if (_commandMask & CommandTypes::MovePlayerFront) {
            _playerMoveDir.x = -1000.0f * sinFacing;
            _playerMoveDir.y = 1000.0f * cosFacing;
            _playerMoveDir.z = 0.0f;
        } else if (_commandMask & CommandTypes::MovePlayerBack) {
            _playerMoveDir.x = 1000.0f * sinFacing;
            _playerMoveDir.y = -1000.0f * cosFacing;
            _playerMoveDir.z = 0.0f;
        } else if (_commandMask & CommandTypes::MovePlayerLeft) {
            _playerMoveDir.x = -1000.0f * cosFacing;
            _playerMoveDir.y = -1000.0f * sinFacing;
            _playerMoveDir.z = 0.0f;
        } else if (_commandMask & CommandTypes::MovePlayerRight) {
            _playerMoveDir.x = 1000.0f * cosFacing;
            _playerMoveDir.y = 1000.0f * sinFacing;
            _playerMoveDir.z = 0.0f;
        } else {
            _playerMoveDir = glm::vec3 {0.0f};
        }
        refreshPlayer();
    } else {
        float sinFacing = glm::sin(_cameraFacing);
        float cosFacing = glm::cos(_cameraFacing);
        float sinPitch = glm::sin(_cameraPitch - glm::half_pi<float>());
        if (_commandMask & CommandTypes::MoveCameraRight) {
            _cameraPosition.x += kCameraMoveRate * cosFacing * dt;
            _cameraPosition.y += kCameraMoveRate * sinFacing * dt;
            _cameraPosition.z += 0.0f;
        } else if (_commandMask & CommandTypes::MoveCameraLeft) {
            _cameraPosition.x -= kCameraMoveRate * cosFacing * dt;
            _cameraPosition.y -= kCameraMoveRate * sinFacing * dt;
            _cameraPosition.z += 0.0f;
        } else if (_commandMask & CommandTypes::MoveCameraFront) {
            _cameraPosition.x -= kCameraMoveRate * sinFacing * dt;
            _cameraPosition.y += kCameraMoveRate * cosFacing * dt;
            _cameraPosition.z += kCameraMoveRate * sinPitch * dt;
        } else if (_commandMask & CommandTypes::MoveCameraBack) {
            _cameraPosition.x += kCameraMoveRate * sinFacing * dt;
            _cameraPosition.y -= kCameraMoveRate * cosFacing * dt;
            _cameraPosition.z -= kCameraMoveRate * sinPitch * dt;
        } else if (_commandMask & CommandTypes::MoveCameraUp) {
            _cameraPosition.x += 0.0f;
            _cameraPosition.y += 0.0f;
            _cameraPosition.z += kCameraMoveRate * dt;
        } else if (_commandMask & CommandTypes::MoveCameraDown) {
            _cameraPosition.x += 0.0f;
            _cameraPosition.y += 0.0f;
            _cameraPosition.z -= kCameraMoveRate * dt;
        }
    }
    refreshCamera();
}

void PlayerCameraController::refreshCamera() {
    if (_playerSceneNode) {
        float sinFacing = glm::sin(_cameraFacing);
        float cosFacing = glm::cos(_cameraFacing);
        float sinPitch = glm::sin(_cameraPitch - glm::half_pi<float>());
        float cosPitch = glm::cos(_cameraPitch - glm::half_pi<float>());
        auto cameraTarget = _playerSceneNode->get().origin() + glm::vec3 {0.0f, 0.0f, kCameraHeight};
        glm::vec3 targetCameraDir {sinFacing * cosPitch,
                                   -cosFacing * cosPitch,
                                   sinPitch};
        Collision collision;
        if (_scene.testLineOfSight(
                cameraTarget,
                cameraTarget + kCameraDistance * targetCameraDir,
                collision)) {
            _cameraPosition = collision.intersection;
        } else {
            _cameraPosition = cameraTarget + kCameraDistance * targetCameraDir;
        }
    }
    auto &camera = _camera->get();
    auto transform = glm::translate(_cameraPosition);
    transform *= glm::eulerAngleZX(_cameraFacing, _cameraPitch);
    camera.setLocalTransform(std::move(transform));
}

void PlayerCameraController::refreshPlayer() {
    (*_gameLogicExecutor)([this]() {
        auto &player = _player->get();
        player.clearAllActions();
        if (glm::length2(_playerMoveDir) > 0.0f) {
            Action action;
            action.type = ActionType::MoveToPoint;
            action.location.position = player.position() + _playerMoveDir;
            player.add(std::move(action));
        } else {
            player.setMoveType(Creature::MoveType::None);
        }
    });
}

} // namespace neo

} // namespace game

} // namespace reone
