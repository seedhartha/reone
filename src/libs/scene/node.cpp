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

#include "reone/scene/node.h"

namespace reone {

namespace scene {

void SceneNode::addChild(SceneNode &node) {
    node._parent = this;
    node.computeAbsoluteTransforms();
    _children.insert(&node);
}

void SceneNode::computeAbsoluteTransforms() {
    if (_parent) {
        _absTransform = _parent->_absTransform * _localTransform;
    } else {
        _absTransform = _localTransform;
    }
    _absTransformInv = glm::inverse(_absTransform);

    for (auto &child : _children) {
        child->computeAbsoluteTransforms();
    }

    onAbsoluteTransformChanged();
}

void SceneNode::removeChild(SceneNode &node) {
    auto maybeChild = _children.find(&node);
    if (maybeChild == _children.end()) {
        return;
    }
    auto child = *maybeChild;
    child->_parent = nullptr;
    child->computeAbsoluteTransforms();
    _children.erase(maybeChild);
}

void SceneNode::removeAllChildren() {
    for (auto &child : _children) {
        child->_parent = nullptr;
        child->computeAbsoluteTransforms();
    }
    _children.clear();
}

void SceneNode::update(float dt) {
    for (auto &child : _children) {
        child->update(dt);
    }
}

glm::vec3 SceneNode::getOrigin() const {
    return glm::vec3(_absTransform[3]);
}

glm::vec2 SceneNode::getOrigin2D() const {
    return glm::vec2(_absTransform[3]);
}

float SceneNode::getDistanceTo(const glm::vec3 &point) const {
    return glm::distance(getOrigin(), point);
}

float SceneNode::getDistanceTo(const SceneNode &other) const {
    return glm::distance(getOrigin(), other.getOrigin());
}

float SceneNode::getSquareDistanceTo(const glm::vec3 &point) const {
    return glm::distance2(getOrigin(), point);
}

float SceneNode::getSquareDistanceTo(const SceneNode &other) const {
    return glm::distance2(getOrigin(), other.getOrigin());
}

float SceneNode::getSquareDistanceTo2D(const glm::vec2 &point) const {
    return glm::distance2(getOrigin2D(), point);
}

glm::vec3 SceneNode::getWorldCenterOfAABB() const {
    return _absTransform * glm::vec4(0.5f * (_aabb.min() + _aabb.max()), 1.0f);
}

void SceneNode::setLocalTransform(glm::mat4 transform) {
    _localTransform = std::move(transform);
    computeAbsoluteTransforms();
}

} // namespace scene

} // namespace reone
