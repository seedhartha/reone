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

#include <stdexcept>

#include "../../common/collectionutil.h"
#include "../../common/log.h"

using namespace std;

namespace reone {

namespace graphics {

Model::Model(
    string name,
    Classification classification,
    shared_ptr<Model> superModel,
    shared_ptr<ModelNode> rootNode,
    float animationScale
) :
    _name(move(name)),
    _classification(classification),
    _superModel(move(superModel)),
    _rootNode(rootNode),
    _animationScale(animationScale) {

    if (!rootNode) {
        throw invalid_argument("rootNode must not be null");
    }

    fillNodeLookups(_rootNode);
    fillBoneNodeId();
    computeAABB();
}

void Model::fillNodeLookups(const shared_ptr<ModelNode> &node) {
    _nodes.push_back(node);
    _nodeById.insert(make_pair(node->id(), node));
    _nodeByName.insert(make_pair(node->name(), node));

    for (auto &child : node->children()) {
        fillNodeLookups(child);
    }
}

void Model::fillBoneNodeId() {
    // In MDL files, bones reference node serial numbers (DFS ordering).
    // We want them to reference node identifiers, for simplicity.

    for (auto &node : _nodes) {
        if (!node->isSkinMesh()) continue;

        shared_ptr<ModelNode::TriangleMesh> mesh(node->mesh());
        mesh->skin->boneNodeId.resize(mesh->skin->boneNodeSerial.size());

        for (size_t i = 0; i < mesh->skin->boneNodeSerial.size(); ++i) {
            uint16_t nodeSerial = mesh->skin->boneNodeSerial[i];
            if (nodeSerial < static_cast<int>(_nodes.size())) {
                mesh->skin->boneNodeId[i] = _nodes[nodeSerial]->id();
            } else {
                mesh->skin->boneNodeId[i] = 0xffff;
            }
        }
    }
}

void Model::computeAABB() {
    _aabb.reset();

    for (auto &node : _nodeById) {
        shared_ptr<ModelNode::TriangleMesh> mesh(node.second->mesh());
        if (mesh) {
            _aabb.expand(mesh->mesh->aabb() * node.second->absoluteTransform());
        }
    }
}

void Model::init() {
    _rootNode->init();
}

void Model::addAnimation(shared_ptr<Animation> animation) {
    _animations.insert(make_pair(animation->name(), move(animation)));
}

shared_ptr<ModelNode> Model::getNodeById(uint16_t nodeId) const {
    return getFromLookupOrNull(_nodeById, nodeId);
}

shared_ptr<ModelNode> Model::getNodeByName(const string &name) const {
    return getFromLookupOrNull(_nodeByName, name);
}

shared_ptr<ModelNode> Model::getAABBNode() const {
    for (auto &node : _nodeById) {
        if (node.second->isAABBMesh()) return node.second;
    }
    return nullptr;
}

shared_ptr<Animation> Model::getAnimation(const string &name) const {
    auto maybeAnim = _animations.find(name);
    if (maybeAnim != _animations.end()) return maybeAnim->second;

    shared_ptr<Animation> anim;
    if (_superModel) {
        anim = _superModel->getAnimation(name);
    }

    return move(anim);
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

set<uint16_t> Model::getAncestorNodes(uint16_t parentId) const {
    set<uint16_t> result;

    auto maybeParent = _nodeById.find(parentId);
    if (maybeParent != _nodeById.end()) {
        for (const ModelNode *node = maybeParent->second->parent(); node; node = node->parent()) {
            result.insert(node->id());
        }
    }

    return move(result);
}

} // namespace graphics

} // namespace reone
