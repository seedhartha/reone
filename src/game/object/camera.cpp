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

#include "camera.h"

#include "../../common/exception/validation.h"
#include "../../graphics/options.h"
#include "../../graphics/services.h"
#include "../../resource/gff.h"
#include "../../scene/collision.h"
#include "../../scene/graph.h"
#include "../../scene/node/camera.h"

#include "../camerastyles.h"
#include "../services.h"

using namespace std;

using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kDefaultClipNear = 0.25f;
static constexpr float kDefaultClipFar = 2500.0f;

static constexpr float kCameraMouseSensitivity = 0.001f;
static constexpr float kCameraRotateSpeed = 1.0f;
static constexpr float kCameraMoveSpeed = 5.0f;

void Camera::loadFromGit(const Gff &git) {
}

void Camera::loadFromStyle(const CameraStyle &style) {
    _style = style;
    _aspect = _graphicsOpt.width / static_cast<float>(_graphicsOpt.height);
    _sceneNode = _sceneGraph->newCamera().get();

    flushProjection();
}

bool Camera::handle(const SDL_Event &e) {
    if (_mode == Mode::ThirdPerson) {
        return handleThirdPerson(e);
    } else if (_mode == Mode::Flycam) {
        return handleFlycam(e);
    } else {
        return false;
    }
}

bool Camera::handleThirdPerson(const SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_a) {
            _left = 1.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_d) {
            _right = 1.0f;
            return true;
        }
    } else if (e.type == SDL_KEYUP) {
        if (e.key.keysym.sym == SDLK_a) {
            _left = 0.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_d) {
            _right = 0.0f;
            return true;
        }
    }
    return false;
}

bool Camera::handleFlycam(const SDL_Event &e) {
    if (e.type == SDL_MOUSEMOTION) {
        _facing -= kCameraMouseSensitivity * static_cast<float>(e.motion.xrel);
        _pitch -= kCameraMouseSensitivity * static_cast<float>(e.motion.yrel);
        return true;
    } else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_w) {
            _forward = 1.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_a) {
            _left = 1.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_s) {
            _backward = 1.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_d) {
            _right = 1.0f;
            return true;
        }
    } else if (e.type == SDL_KEYUP) {
        if (e.key.keysym.sym == SDLK_w) {
            _forward = 0.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_a) {
            _left = 0.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_s) {
            _backward = 0.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_d) {
            _right = 0.0f;
            return true;
        }
    }
    return false;
}

void Camera::update(float delta) {
    if (_mode == Mode::ThirdPerson) {
        updateThirdPerson(delta);
    } else if (_mode == Mode::Flycam) {
        updateFlycam(delta);
    }
}

void Camera::updateThirdPerson(float delta) {
    // Orientation

    _facing += delta * kCameraRotateSpeed * (_left - _right);
    _facing = glm::mod(_facing, glm::two_pi<float>());

    _pitch = glm::radians(_style.pitch);

    // Position

    auto startPos = _hook->getOrigin();

    auto endPos = startPos;
    endPos += glm::vec3(0.0f, 0.0f, _style.height);
    endPos += _style.distance * glm::vec3(glm::sin(_facing), -glm::cos(_facing), 0.0f);

    Collision collision;
    if (_sceneGraph->testLineOfSight(startPos, endPos, collision)) {
        endPos = collision.intersection;
    }

    _position = move(endPos);

    //

    flushTransform();
}

void Camera::updateFlycam(float delta) {
    auto rotation = glm::rotate(_facing, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(_pitch, glm::vec3(1.0f, 0.0f, 0.0f));

    _position += delta * kCameraMoveSpeed * glm::vec3(rotation * glm::vec4(_right - _left, 0.0f, _backward - _forward, 1.0f));

    flushTransform();
}

void Camera::flushProjection() {
    static_cast<CameraSceneNode &>(*_sceneNode).setPerspectiveProjection(glm::radians(_style.viewAngle), //
                                                                         _aspect,                        //
                                                                         kDefaultClipNear,               //
                                                                         kDefaultClipFar);
}

} // namespace game

} // namespace reone
