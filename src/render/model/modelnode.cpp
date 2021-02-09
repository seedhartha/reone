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

#include "glm/gtx/matrix_decompose.hpp"

#include "../../common/log.h"

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

void ModelNode::addChild(shared_ptr<ModelNode> child) {
    child->_parent = this;
    _children.push_back(move(child));
}

void ModelNode::computeLocalTransforms() {
    if (_parent) {
        _localTransform = glm::inverse(_parent->_absTransform) * _absTransform;
        _absTransform = _parent->_absTransform * _localTransform;
        _absTransformInv = glm::inverse(_absTransform);

        // Extract position and orientation for use in animations.
        glm::vec3 scale, skew;
        glm::vec4 perspective;
        glm::decompose(_localTransform, scale, _orientation, _position, skew, perspective);

    } else {
        _localTransform = _absTransform;
    }

    _absTransform = _parent ? _parent->_absTransform * _localTransform : _localTransform;
    _absTransformInv = glm::inverse(_absTransform);

    for (auto &child : _children) {
        child->computeLocalTransforms();
    }
}

void ModelNode::computeAbsoluteTransforms() {
    _absTransform = _parent ? _parent->_absTransform * _localTransform : _localTransform;
    _absTransformInv = glm::inverse(_absTransform);

    for (auto &child : _children) {
        child->computeAbsoluteTransforms();
    }
}

void ModelNode::addTranslationKeyframe(TranslationKeyframe keyframe) {
    _translationFrames.push_back(move(keyframe));
}

void ModelNode::addOrientationKeyframe(OrientationKeyframe keyframe) {
    _orientationFrames.push_back(move(keyframe));
}

bool ModelNode::getTranslation(float time, glm::vec3 &translation, float scale) const {
    if (_translationFrames.empty()) return false;

    const TranslationKeyframe *left = &_translationFrames.front();
    const TranslationKeyframe *right = left;

    for (auto it = _translationFrames.begin(); it != _translationFrames.end(); ++it) {
        if (it->time >= time) {
            right = &*it;
            if (it != _translationFrames.begin()) left = &*(it - 1);
            break;
        }
    }

    if (left == right) {
        translation = left->translation * scale;
        return true;
    }

    float factor = (time - left->time) / (right->time - left->time);

    translation = glm::mix(left->translation, right->translation, factor) * scale;

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
    return _mesh->mesh()->aabb().center();
}

void ModelNode::setName(string name) {
    _name = move(name);
}

void ModelNode::setNodeNumber(uint16_t nodeNumber) {
    _nodeNumber = nodeNumber;
}

void ModelNode::setAbsoluteTransform(glm::mat4 transform) {
    _absTransform = move(transform);
    _absTransformInv = glm::inverse(_absTransform);
}

void ModelNode::setMesh(shared_ptr<ModelMesh> mesh) {
    _mesh = move(mesh);
}

void ModelNode::setSkin(shared_ptr<Skin> skin) {
    _skin = move(skin);
}

} // namespace render

} // namespace reone
