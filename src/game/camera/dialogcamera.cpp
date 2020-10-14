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

#include "dialogcamera.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace game {

DialogCamera::DialogCamera(SceneGraph *sceneGraph, const CameraStyle &style, float aspect, float zNear, float zFar) {
    glm::mat4 projection(glm::perspective(glm::radians(style.viewAngle), aspect, zNear, zFar));
    _sceneNode = make_shared<CameraSceneNode>(sceneGraph, projection);
}

void DialogCamera::setSpeakerPosition(const glm::vec3 &position) {
    if (_speakerPosition != position) {
        _speakerPosition = position;
        updateSceneNode();
    }
}

void DialogCamera::setListenerPosition(const glm::vec3 &position) {
    if (_listenerPosition != position) {
        _listenerPosition = position;
        updateSceneNode();
    }
}

void DialogCamera::setVariant(Variant variant) {
    if (_variant != variant) {
        _variant = variant;
        updateSceneNode();
    }
}

void DialogCamera::setFindObstacle(const function<bool(const glm::vec3 &, const glm::vec3 &, glm::vec3 &)> &fn) {
    _findObstacle = fn;
}

void DialogCamera::updateSceneNode() {
    static glm::vec3 up(0.0f, 0.0f, 1.0f);
    static glm::vec3 down(0.0f, 0.0f, -1.0f);

    glm::vec3 listenerToSpeaker(_speakerPosition - _listenerPosition);
    float distance = glm::length(listenerToSpeaker);
    glm::vec3 dir(glm::normalize(listenerToSpeaker));
    glm::vec3 center(0.5f * (_listenerPosition + _speakerPosition));

    glm::vec3 eye(0.0f);
    glm::vec3 target(0.0f);
    switch (_variant) {
        case Variant::SpeakerClose:
            eye = center - 0.25f * distance * dir;
            eye += glm::min(0.25f * glm::length(listenerToSpeaker), 1.0f) * glm::cross(dir, down);
            eye += glm::vec3(0.0f, 0.0f, 0.1f);

            target = _speakerPosition;
            target += glm::vec3(0.0f, 0.0f, 0.1f);
            break;
        case Variant::ListenerClose:
            eye = center + 0.25f * distance * dir;
            eye += glm::min(0.25f * glm::length(listenerToSpeaker), 1.0f) * glm::cross(dir, down);
            eye += glm::vec3(0.0f, 0.0f, 0.1f);

            target = _listenerPosition;
            target += glm::vec3(0.0f, 0.0f, 0.1f);
            break;
        case Variant::Both:
        default:
            eye = center;
            eye += glm::min(2.25f * glm::length(listenerToSpeaker), 4.0f) * glm::cross(dir, down);
            eye += glm::vec3(0.0f, 0.0f, 0.25f);

            target = center;
            target -= glm::vec3(0.0f, 0.0f, 0.25f);
            break;
    }
    if (_findObstacle) {
        static glm::vec3 intersection;
        if (_findObstacle(target, eye, intersection)) {
            eye = intersection;
        }
    }
    glm::mat4 transform(1.0f);
    transform *= glm::inverse(glm::lookAt(eye, target, up));

    _sceneNode->setLocalTransform(transform);
}

} // namespace game

} // namespace reone
