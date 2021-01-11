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

const glm::vec3 &ModelNode::getCenterOfAABB() const {
    return _mesh->aabb().center();
}

int ModelNode::index() const {
    return _index;
}

const ModelNode *ModelNode::parent() const {
    return _parent;
}

uint16_t ModelNode::nodeNumber() const {
    return _nodeNumber;
}

const string &ModelNode::name() const {
    return _name;
}

const glm::vec3 &ModelNode::position() const {
    return _position;
}

const glm::quat &ModelNode::orientation() const {
    return _orientation;
}

const glm::mat4 &ModelNode::localTransform() const {
    return _localTransform;
}

const glm::mat4 &ModelNode::absoluteTransform() const {
    return _absTransform;
}

const glm::mat4 &ModelNode::absoluteTransformInverse() const {
    return _absTransformInv;
}

const glm::vec3 &ModelNode::color() const {
    return _color;
}

bool ModelNode::isSelfIllumEnabled() const {
    return _selfIllumEnabled;
}

const glm::vec3 &ModelNode::selfIllumColor() const {
    return _selfIllumColor;
}

float ModelNode::alpha() const {
    return _alpha;
}

float ModelNode::radius() const {
    return _radius;
}

float ModelNode::multiplier() const {
    return _multiplier;
}

shared_ptr<ModelNode::Light> ModelNode::light() const {
    return _light;
}

shared_ptr<ModelMesh> ModelNode::mesh() const {
    return _mesh;
}

shared_ptr<ModelNode::Skin> ModelNode::skin() const {
    return _skin;
}

shared_ptr<Emitter> ModelNode::emitter() const {
    return _emitter;
}

const vector<shared_ptr<ModelNode>> &ModelNode::children() const {
    return _children;
}

} // namespace render

} // namespace reone
