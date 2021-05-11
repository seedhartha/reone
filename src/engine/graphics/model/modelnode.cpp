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

namespace graphics {

ModelNode::ModelNode(int index, const ModelNode *parent) : _index(index), _parent(parent) {
}

void ModelNode::init() {
    if (_mesh) {
        _mesh->mesh->init();
    }
    for (auto &child : _children) {
        child->init();
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

bool ModelNode::getPosition(int leftFrameIdx, int rightFrameIdx, float factor, glm::vec3 &position) const {
    if (leftFrameIdx < 0 || leftFrameIdx >= static_cast<int>(_positions.getNumKeyframes()) ||
        rightFrameIdx < 0 || rightFrameIdx >= static_cast<int>(_positions.getNumKeyframes())) return false;

    if (leftFrameIdx == rightFrameIdx) {
        position = _positions.getByKeyframe(leftFrameIdx);
    } else {
        position = glm::mix(
            _positions.getByKeyframe(leftFrameIdx),
            _positions.getByKeyframe(rightFrameIdx),
            factor);
    }

    return true;
}

bool ModelNode::getOrientation(int leftFrameIdx, int rightFrameIdx, float interpolant, glm::quat &orientation) const {
    if (leftFrameIdx < 0 || leftFrameIdx >= static_cast<int>(_orientations.getNumKeyframes()) ||
        rightFrameIdx < 0 || rightFrameIdx >= static_cast<int>(_orientations.getNumKeyframes())) return false;

    if (leftFrameIdx == rightFrameIdx) {
        orientation = _orientations.getByKeyframe(leftFrameIdx);
    } else {
        orientation = glm::slerp(
            _orientations.getByKeyframe(leftFrameIdx),
            _orientations.getByKeyframe(rightFrameIdx),
            interpolant);
    }

    return true;
}

bool ModelNode::getScale(int leftFrameIdx, int rightFrameIdx, float interpolant, float &scale) const {
    if (leftFrameIdx < 0 || leftFrameIdx >= static_cast<int>(_scales.getNumKeyframes()) ||
        rightFrameIdx < 0 || rightFrameIdx >= static_cast<int>(_scales.getNumKeyframes())) return false;

    if (leftFrameIdx == rightFrameIdx) {
        scale = _scales.getByKeyframe(leftFrameIdx);
    } else {
        scale = glm::mix(
            _scales.getByKeyframe(leftFrameIdx),
            _scales.getByKeyframe(rightFrameIdx),
            interpolant);
    }

    return true;
}

const glm::vec3 &ModelNode::getCenterOfAABB() const {
    return _mesh->mesh->aabb().center();
}

} // namespace graphics

} // namespace reone
