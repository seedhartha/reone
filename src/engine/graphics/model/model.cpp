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

#include "model.h"

#include "../../common/collectionutil.h"
#include "../../common/guardutil.h"
#include "../../common/logutil.h"

#include "../mesh/mesh.h"

#include "animation.h"
#include "modelnode.h"

using namespace std;

namespace reone {

namespace graphics {

Model::Model(
    string name,
    int classification,
    shared_ptr<ModelNode> rootNode,
    vector<shared_ptr<Animation>> animations,
    shared_ptr<Model> superModel,
    float animationScale) :
    _name(move(name)),
    _classification(classification),
    _rootNode(ensurePresent(rootNode, "rootNode")),
    _superModel(move(superModel)),
    _animationScale(animationScale) {

    fillNodeByName(_rootNode);
    computeAABB();

    for (auto &anim : animations) {
        _animations.insert(make_pair(anim->name(), anim));
    }
}

void Model::fillNodeByName(const shared_ptr<ModelNode> &node) {
    _nodeByName.insert(make_pair(node->name(), node));

    for (auto &child : node->children()) {
        fillNodeByName(child);
    }
}

void Model::computeAABB() {
    _aabb.reset();

    for (auto &node : _nodeByName) {
        shared_ptr<ModelNode::TriangleMesh> mesh(node.second->mesh());
        if (mesh) {
            _aabb.expand(mesh->mesh->aabb() * node.second->absoluteTransform());
        }
    }
}

void Model::init() {
    _rootNode->init();
}

shared_ptr<ModelNode> Model::getNodeByName(const string &name) const {
    return getFromLookupOrNull(_nodeByName, name);
}

shared_ptr<ModelNode> Model::getNodeByNameRecursive(const string &name) const {
    auto result = getFromLookupOrNull(_nodeByName, name);
    if (!result && _superModel) {
        result = _superModel->getNodeByNameRecursive(name);
    }
    return move(result);
}

shared_ptr<ModelNode> Model::getAABBNode() const {
    for (auto &node : _nodeByName) {
        if (node.second->isAABBMesh())
            return node.second;
    }
    return nullptr;
}

set<string> Model::getAncestorNodes(const string &parentName) const {
    set<string> result;

    auto maybeParent = _nodeByName.find(parentName);
    if (maybeParent != _nodeByName.end()) {
        for (const ModelNode *node = maybeParent->second->parent(); node; node = node->parent()) {
            result.insert(node->name());
        }
    }

    return move(result);
}

vector<string> Model::getAnimationNames() const {
    vector<string> result;

    if (_superModel) {
        vector<string> superAnims(_superModel->getAnimationNames());
        for (auto &anim : superAnims) {
            result.push_back(anim);
        }
    }
    for (auto &anim : _animations) {
        result.push_back(anim.first);
    }

    return move(result);
}

shared_ptr<Animation> Model::getAnimation(const string &name) const {
    auto maybeAnim = _animations.find(name);
    if (maybeAnim != _animations.end())
        return maybeAnim->second;

    shared_ptr<Animation> anim;
    if (_superModel) {
        anim = _superModel->getAnimation(name);
    }

    return move(anim);
}

} // namespace graphics

} // namespace reone
