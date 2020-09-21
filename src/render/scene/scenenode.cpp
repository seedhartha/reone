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

#include "scenenode.h"

#include "glm/gtx/norm.hpp"

#include "scenegraph.h"

using namespace std;

namespace reone {

namespace render {

void SceneNode::addChild(const shared_ptr<SceneNode> &node) {
    assert(node);
    node->setParent(this);
    _children.push_back(node);
}

void SceneNode::fillSceneGraph() {
    for (auto &child : _children) {
        child->fillSceneGraph();
    }
}

void SceneNode::render() const {
    for (auto &child : _children) {
        child->render();
    }
}

float SceneNode::distanceTo(const glm::vec3 &point) const {
    return glm::distance2(glm::vec3(_absoluteTransform[3]), point);
}

const SceneNode *SceneNode::parent() const {
    return _parent;
}

const glm::mat4 &SceneNode::localTransform() const {
    return _localTransform;
}

const glm::mat4 &SceneNode::absoluteTransform() const {
    return _absoluteTransform;
}

const vector<shared_ptr<SceneNode>> &SceneNode::children() const {
    return _children;
}

void SceneNode::setParent(const SceneNode *parent) {
    _parent = parent;
    updateAbsoluteTransform();
}

void SceneNode::updateAbsoluteTransform() {
    _absoluteTransform = _parent ? _parent->_absoluteTransform : glm::mat4(1.0f);
    _absoluteTransform *= _localTransform;

    for (auto &child : _children) {
        child->updateAbsoluteTransform();
    }
}

void SceneNode::setLocalTransform(const glm::mat4 &transform) {
    _localTransform = transform;
    updateAbsoluteTransform();
}

} // namespace render

} // namespace reone
