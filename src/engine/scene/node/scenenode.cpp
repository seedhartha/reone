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

#include "scenenode.h"

using namespace std;

namespace reone {

namespace scene {

void SceneNode::addChild(shared_ptr<SceneNode> node) {
    node->_parent = this;
    node->computeAbsoluteTransforms();
    _children.push_back(node);
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
    auto maybeChild = find_if(
        _children.begin(),
        _children.end(),
        [&node](auto &n) { return n.get() == &node; });

    if (maybeChild != _children.end()) {
        node._parent = nullptr;
        node.computeAbsoluteTransforms();
        _children.erase(maybeChild);
    }
}

void SceneNode::update(float dt) {
    for (auto &child : _children) {
        child->update(dt);
    }
}

void SceneNode::draw() {
    for (auto &child : _children) {
        child->draw();
    }
}

glm::vec3 SceneNode::getOrigin() const {
    return glm::vec3(_absTransform[3]);
}

float SceneNode::getDistanceTo(const glm::vec3 &point) const {
    return glm::distance(getOrigin(), point);
}

float SceneNode::getDistanceTo2(const glm::vec3 &point) const {
    return glm::distance2(getOrigin(), point);
}

float SceneNode::getDistanceTo(const SceneNode &other) const {
    return glm::distance(getOrigin(), other.getOrigin());
}

float SceneNode::getDistanceTo2(const SceneNode &other) const {
    return glm::distance2(getOrigin(), other.getOrigin());
}

glm::vec3 SceneNode::getWorldCenterOfAABB() const {
    return _absTransform * glm::vec4(_aabb.center(), 1.0f);
}

void SceneNode::setLocalTransform(glm::mat4 transform) {
    _localTransform = move(transform);
    computeAbsoluteTransforms();
}

} // namespace scene

} // namespace reone
