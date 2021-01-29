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

#include "modelnode.h"

using namespace std;

namespace reone {

namespace render {

ModelNode::ModelNode(int index, const ModelNode *parent) : _index(index), _parent(parent) {
}

void ModelNode::initGL() {
    if (_mesh) {
        _mesh->initGL();
    }
    for (auto &child : _children) {
        child->initGL();
    }
}

bool ModelNode::getPosition(float time, glm::vec3 &position, float scale) const {
    if (_positionFrames.empty()) return false;

    const PositionKeyframe *left = &_positionFrames.front();
    const PositionKeyframe *right = left;

    for (auto it = _positionFrames.begin(); it != _positionFrames.end(); ++it) {
        if (it->time >= time) {
            right = &*it;
            if (it != _positionFrames.begin()) left = &*(it - 1);
            break;
        }
    }

    if (left == right) {
        position = left->position * scale;
        return true;
    }

    float factor = (time - left->time) / (right->time - left->time);

    position = glm::mix(left->position, right->position, factor) * scale;

    return true;
}

bool ModelNode::getOrientation(float time, glm::quat &orientation) const {
    if (_orientationFrames.empty()) return false;

    const OrientationKeyframe *left = &_orientationFrames.front();
    const OrientationKeyframe *right = left;

    for (auto it = _orientationFrames.begin(); it != _orientationFrames.end(); ++it) {
        if (it->time >= time) {
            right = &*it;
            if (it != _orientationFrames.begin()) left = &*(it - 1);
            break;
        }
    }

    if (left == right) {
        orientation = left->orientation;
        return true;
    }

    float factor = (time - left->time) / (right->time - left->time);

    orientation = glm::slerp(left->orientation, right->orientation, factor);

    return true;
}

bool ModelNode::getScale(float time, float &scale) const {
    if (_scaleFrames.empty()) return false;

    const ScaleKeyframe *left = &_scaleFrames.front();
    const ScaleKeyframe *right = left;

    for (auto it = _scaleFrames.begin(); it != _scaleFrames.end(); ++it) {
        if (it->time >= time) {
            right = &*it;
            if (it != _scaleFrames.begin()) left = &*(it - 1);
            break;
        }
    }

    if (left == right) {
        scale = left->scale;
        return true;
    }

    float factor = (time - left->time) / (right->time - left->time);

    scale = glm::mix(left->scale, right->scale, factor);

    return true;
}

const glm::vec3 &ModelNode::getCenterOfAABB() const {
    return _mesh->aabb().center();
}

} // namespace render

} // namespace reone
