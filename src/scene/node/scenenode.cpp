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

#include <algorithm>

#include "glm/gtx/norm.hpp"

#include "../scenegraph.h"

using namespace std;

namespace reone {

namespace scene {

SceneNode::SceneNode(SceneNodeType type, SceneGraph *sceneGraph) : _type(type), _sceneGraph(sceneGraph) {
}

void SceneNode::addChild(const shared_ptr<SceneNode> &node) {
    node->setParent(this);
    _children.push_back(node);
}

void SceneNode::removeChild(SceneNode &node) {
    auto maybeChild = find_if(
        _children.begin(),
        _children.end(),
        [&node](const shared_ptr<SceneNode> &n) { return n.get() == &node; });

    if (maybeChild != _children.end()) {
        node.setParent(nullptr);
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

void SceneNode::drawSingle(bool shadowPass) {
}

glm::vec3 SceneNode::getOrigin() const {
    return glm::vec3(_absoluteTransform[3]);
}

float SceneNode::getDistanceTo(const glm::vec3 &point) const {
    return glm::distance(getOrigin(), point);
}

float SceneNode::getDistanceTo2(const glm::vec3 &point) const {
    return glm::distance2(getOrigin(), point);
}

float SceneNode::getDistanceTo(const SceneNode &other) const {
    return glm::sqrt(getDistanceTo2(other));
}

float SceneNode::getDistanceTo2(const SceneNode &other) const {
    if (!other.isVolumetric()) {
        return glm::distance2(getOrigin(), other.getOrigin());
    }

    glm::vec3 aabbSpaceOrigin(other._absoluteTransformInv * glm::vec4(getOrigin(), 1.0f));

    return other.aabb().getDistanceFromClosestPoint2(aabbSpaceOrigin);
}

void SceneNode::setParent(const SceneNode *parent) {
    _parent = parent;
    updateAbsoluteTransform();
}

void SceneNode::updateAbsoluteTransform() {
    _absoluteTransform = _parent ? _parent->_absoluteTransform : glm::mat4(1.0f);
    _absoluteTransform *= _localTransform;
    _absoluteTransformInv = glm::inverse(_absoluteTransform);

    for (auto &child : _children) {
        child->updateAbsoluteTransform();
    }
}

void SceneNode::setLocalTransform(const glm::mat4 &transform) {
    _localTransform = transform;
    updateAbsoluteTransform();
}

void SceneNode::setPosition(glm::vec3 position) {
    setLocalTransform(glm::translate(glm::mat4(1.0f), position));
}

void SceneNode::setVisible(bool visible) {
    _visible = visible;
}

void SceneNode::setTransparent(bool transparent) {
    _transparent = transparent;
}

} // namespace scene

} // namespace reone
