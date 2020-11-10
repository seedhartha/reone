/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "octree.h"

#include <algorithm>
#include <stdexcept>
#include <stack>

#include "cameranode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static const float kMinNodeSize = 8.0f;

OctreeNode::OctreeNode(OctreeNode *parent, AABB aabb) : _parent(parent), _aabb(move(aabb)) {
    computeQuadrantsAABB();
}

void OctreeNode::computeQuadrantsAABB() {
    _bottomLeftAabb = AABB(
        glm::vec3(_aabb.min().x, _aabb.center().y, _aabb.min().z),
        glm::vec3(_aabb.center().x, _aabb.max().y, _aabb.max().z));

    _bottomRightAabb = AABB(
        glm::vec3(_aabb.center().x, _aabb.center().y, _aabb.min().z),
        glm::vec3(_aabb.max().x, _aabb.max().y, _aabb.max().z));

    _topLeftAabb = AABB(
        glm::vec3(_aabb.min().x, _aabb.min().y, _aabb.min().z),
        glm::vec3(_aabb.center().x, _aabb.center().y, _aabb.max().z));

    _topRightAabb = AABB(
        glm::vec3(_aabb.center().x, _aabb.min().y, _aabb.min().z),
        glm::vec3(_aabb.max().x, _aabb.center().y, _aabb.max().z));
}

void OctreeNode::add(OctreeObject *object) {
    if (isLeaf()) {
        object->nodes.insert(this);
        _objects.insert(object);
        return;
    }
    if (isObjectIn(object, Quadrant::BottomLeft)) {
        if (!_bottomLeft) {
            _bottomLeft = make_unique<OctreeNode>(this, _bottomLeftAabb);
        }
        _bottomLeft->add(object);
    }
    if (isObjectIn(object, Quadrant::BottomRight)) {
        if (!_bottomRight) {
            _bottomRight = make_unique<OctreeNode>(this, _bottomRightAabb);
        }
        _bottomRight->add(object);
    }
    if (isObjectIn(object, Quadrant::TopLeft)) {
        if (!_topLeft) {
            _topLeft = make_unique<OctreeNode>(this, _topLeftAabb);
        }
        _topLeft->add(object);
    }
    if (isObjectIn(object, Quadrant::TopRight)) {
        if (!_topRight) {
            _topRight = make_unique<OctreeNode>(this, _topRightAabb);
        }
        _topRight->add(object);
    }
}

bool OctreeNode::isLeaf() const {
    return glm::length(glm::vec2(_aabb.max() - _aabb.min())) < kMinNodeSize;
}

bool OctreeNode::isObjectIn(OctreeObject *object, Quadrant quadrant) const {
    const AABB *aabb;
    switch (quadrant) {
        case Quadrant::BottomLeft:
            aabb = &_bottomLeftAabb;
            break;
        case Quadrant::BottomRight:
            aabb = &_bottomRightAabb;
            break;
        case Quadrant::TopLeft:
            aabb = &_topLeftAabb;
            break;
        case Quadrant::TopRight:
            aabb = &_topRightAabb;
            break;
        default:
            throw invalid_argument("Invalid quadrant");
    }
    return aabb->intersect(object->aabb);
}

void Octree::clear() {
    _root.reset();
    _objects.clear();
}

void Octree::registerObject(OctreeObject object) {
    _objects.insert(make_pair(object.sceneNode, move(object)));
}

void Octree::build() {
    AABB aabb;
    for (auto &object : _objects) {
        aabb.expand(object.second.aabb);
    }
    _root = make_unique<OctreeNode>(nullptr, aabb);

    for (auto &object : _objects) {
        _root->add(&object.second);
    }
}

vector<SceneNode *> Octree::getNodesInFrustum(const CameraSceneNode *camera) const {
    stack<OctreeNode *> nodes;
    nodes.push(_root.get());

    unordered_map<SceneNode *, float> cameraDistByNode;
    while (!nodes.empty()) {
        OctreeNode *node = nodes.top();
        nodes.pop();

        if (!camera->isInFrustum(node->_aabb)) continue;

        float dist = camera->distanceTo(node->_aabb.center());
        if (!node->_objects.empty()) {
            for (auto &object : node->_objects) {
                auto maybeDistance = cameraDistByNode.find(object->sceneNode);
                if (maybeDistance != cameraDistByNode.end()) {
                    if (dist > maybeDistance->second) {
                        cameraDistByNode[object->sceneNode] = dist;
                    }
                    continue;
                }
                cameraDistByNode.insert(make_pair(object->sceneNode, dist));
            }
        }
        if (node->_bottomLeft) {
            nodes.push(node->_bottomLeft.get());
        }
        if (node->_bottomRight) {
            nodes.push(node->_bottomRight.get());
        }
        if (node->_topLeft) {
            nodes.push(node->_topLeft.get());
        }
        if (node->_topRight) {
            nodes.push(node->_topRight.get());
        }
    }

    vector<pair<SceneNode *, float>> cameraDistances;
    for (auto &pair : cameraDistByNode) {
        cameraDistances.push_back(pair);
    }
    sort(cameraDistances.begin(), cameraDistances.end(), [](auto &left, auto &right) {
        return left.second > right.second;
    });
    vector<SceneNode *> result;
    for (auto &pair : cameraDistances) {
        result.push_back(pair.first);
    }

    return move(result);
}

} // namespace scene

} // namespace reone
