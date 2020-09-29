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

#include "firstperson.h"

#include "glm/ext.hpp"

using namespace std;

namespace reone {

namespace render {

static const float kMovementSpeed = 5.0f;
static const float kMouseMultiplier = glm::pi<float>() / 2000.0f;

FirstPersonCamera::FirstPersonCamera(SceneGraph *sceneGraph, float aspect, float fovy, float zNear, float zFar) {
    _sceneNode = make_unique<CameraSceneNode>(sceneGraph, fovy, aspect, zNear, zFar);
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
    _heading = glm::mod(
        _heading - event.xrel * kMouseMultiplier,
        glm::two_pi<float>());

    constexpr float quarterPi = glm::quarter_pi<float>();

    _pitch = glm::clamp(
        _pitch - event.yrel * kMouseMultiplier,
        -quarterPi,
        quarterPi);

    updateView();

    return true;
}

void FirstPersonCamera::updateView() {
    glm::quat orientation(glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f));
    orientation *= glm::quat(glm::vec3(_pitch, _heading, 0.0f));

    _sceneNode->setLocalTransform(glm::translate(glm::mat4(1.0f), _position) * glm::mat4_cast(orientation));
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

        default:
            return false;
    }
}

void FirstPersonCamera::update(float dt) {
    bool positionChanged = false;
    float sinYawAdj = glm::sin(_heading) * kMovementSpeed * dt;
    float cosYawAdj = glm::cos(_heading) * kMovementSpeed * dt;
    float sinPitchAdj = glm::sin(_pitch) * kMovementSpeed * dt;

    if (_moveForward) {
        _position.x -= sinYawAdj;
        _position.y += cosYawAdj;
        _position.z += sinPitchAdj;
        positionChanged = true;
    }
    if (_moveLeft) {
        _position.x -= cosYawAdj;
        _position.y -= sinYawAdj;
        positionChanged = true;
    }
    if (_moveBackward) {
        _position.x += sinYawAdj;
        _position.y -= cosYawAdj;
        _position.z -= sinPitchAdj;
        positionChanged = true;
    }
    if (_moveRight) {
        _position.x += cosYawAdj;
        _position.y += sinYawAdj;
        positionChanged = true;
    }

    if (positionChanged) updateView();
}

void FirstPersonCamera::resetInput() {
    _moveForward = false;
    _moveLeft = false;
    _moveBackward = false;
    _moveRight = false;
}

void FirstPersonCamera::setPosition(const glm::vec3 &pos) {
    _position = pos;
    updateView();
}

void FirstPersonCamera::setHeading(float heading) {
    _heading = heading;
    updateView();
}

} // namespace render

} // namespace reone
