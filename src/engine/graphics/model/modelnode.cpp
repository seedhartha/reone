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

#include "../../common/log.h"

#include "glm/gtx/transform.hpp"

using namespace std;

namespace reone {

namespace graphics {

ModelNode::ModelNode(
    string name,
    glm::vec3 restPosition,
    glm::quat restOrientation,
    const ModelNode *parent
) :
    _name(move(name)),
    _restPosition(move(restPosition)),
    _restOrientation(move(restOrientation)),
    _parent(parent) {

    computeLocalTransform();
    computeAbsoluteTransform();
}

void ModelNode::computeLocalTransform() {
    _localTransform = glm::mat4(1.0f);
    _localTransform *= glm::translate(_restPosition);
    _localTransform *= glm::mat4_cast(_restOrientation);
}

void ModelNode::computeAbsoluteTransform() {
    if (_parent) {
        _absTransform = _parent->_absTransform * _localTransform;
    } else {
        _absTransform = _localTransform;
    }
    _absTransformInv = glm::inverse(_absTransform);
}

void ModelNode::init() {
    if (_mesh) {
        _mesh->mesh->init();
    }
    for (auto &child : _children) {
        child->init();
    }
}

void ModelNode::addChild(std::shared_ptr<ModelNode> child) {
    _children.push_back(move(child));
}

bool ModelNode::getPosition(int leftFrameIdx, int rightFrameIdx, float factor, glm::vec3 &position) const {
    if (leftFrameIdx < 0 || leftFrameIdx >= static_cast<int>(_position.getNumFrames()) ||
        rightFrameIdx < 0 || rightFrameIdx >= static_cast<int>(_position.getNumFrames())) return false;

    if (leftFrameIdx == rightFrameIdx) {
        position = _position.getByFrame(leftFrameIdx);
    } else {
        position = glm::mix(
            _position.getByFrame(leftFrameIdx),
            _position.getByFrame(rightFrameIdx),
            factor);
    }

    return true;
}

bool ModelNode::getOrientation(int leftFrameIdx, int rightFrameIdx, float factor, glm::quat &orientation) const {
    if (leftFrameIdx < 0 || leftFrameIdx >= static_cast<int>(_orientation.getNumFrames()) ||
        rightFrameIdx < 0 || rightFrameIdx >= static_cast<int>(_orientation.getNumFrames())) return false;

    if (leftFrameIdx == rightFrameIdx) {
        orientation = _orientation.getByFrame(leftFrameIdx);
    } else {
        orientation = glm::slerp(
            _orientation.getByFrame(leftFrameIdx),
            _orientation.getByFrame(rightFrameIdx),
            factor);
    }

    return true;
}

bool ModelNode::getScale(int leftFrameIdx, int rightFrameIdx, float factor, float &scale) const {
    if (leftFrameIdx < 0 || leftFrameIdx >= static_cast<int>(_scale.getNumFrames()) ||
        rightFrameIdx < 0 || rightFrameIdx >= static_cast<int>(_scale.getNumFrames())) return false;

    if (leftFrameIdx == rightFrameIdx) {
        scale = _scale.getByFrame(leftFrameIdx);
    } else {
        scale = glm::mix(
            _scale.getByFrame(leftFrameIdx),
            _scale.getByFrame(rightFrameIdx),
            factor);
    }

    return true;
}

vector<uint32_t> ModelNode::getFacesByMaterial(uint32_t material) const {
    if (!_mesh || _mesh->materialFaces.count(material) == 0) return vector<uint32_t>();

    return _mesh->materialFaces.find(material)->second;
}

} // namespace graphics

} // namespace reone
