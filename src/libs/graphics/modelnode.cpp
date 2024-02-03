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

#include "reone/graphics/modelnode.h"

#include "reone/graphics/mesh.h"
#include "reone/graphics/texture.h"
#include "reone/system/logutil.h"

namespace reone {

namespace graphics {

ModelNode::ModelNode(
    uint16_t number,
    std::string name,
    glm::vec3 restPosition,
    glm::quat restOrientation,
    bool animated,
    ModelNode *parent) :
    _number(number),
    _name(std::move(name)),
    _restPosition(std::move(restPosition)),
    _restOrientation(std::move(restOrientation)),
    _animated(animated),
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
    if (_mesh && _mesh->mesh) {
        _mesh->mesh->init();
    }
    for (auto &child : _children) {
        child->init();
    }
}

void ModelNode::addChild(std::shared_ptr<ModelNode> child) {
    _children.push_back(std::move(child));
}

bool ModelNode::floatValueAtTime(ControllerType type, float time, float &value) const {
    if (_floatTracks.count(type) == 0) {
        return false;
    }
    const auto &track = _floatTracks.at(type);
    return track.valueAtTime(time, value);
}

bool ModelNode::vectorValueAtTime(ControllerType type, float time, glm::vec3 &value) const {
    if (_vectorTracks.count(type) == 0) {
        return false;
    }
    const auto &track = _vectorTracks.at(type);
    return track.valueAtTime(time, value);
}

bool ModelNode::quaternionValueAt(ControllerType type, float time, glm::quat &value) const {
    if (_quaternionTracks.count(type) == 0) {
        return false;
    }
    const auto &track = _quaternionTracks.at(type);
    return track.valueAtTime(time, value);
}

} // namespace graphics

} // namespace reone
