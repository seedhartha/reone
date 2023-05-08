/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/common/collectionutil.h"
#include "reone/common/logutil.h"

#include "animation.h"
#include "mesh.h"
#include "modelnode.h"
#include "types.h"

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
    _rootNode(rootNode),
    _superModel(move(superModel)),
    _animationScale(animationScale) {

    if (_rootNode) {
        fillLookups(_rootNode);
    }
    computeAABB();

    for (auto &anim : animations) {
        _animations[anim->name()] = anim;
    }
}

void Model::fillLookups(const shared_ptr<ModelNode> &node) {
    _nodeByNumber[node->number()] = node;
    _nodeByName[node->name()] = node;

    for (auto &child : node->children()) {
        fillLookups(child);
    }
}

void Model::computeAABB() {
    _aabb.reset();

    for (auto &node : _nodeByNumber) {
        auto mesh = node.second->mesh();
        if (mesh && mesh->mesh) {
            _aabb.expand(mesh->mesh->aabb() * node.second->absoluteTransform());
        }
    }
}

void Model::init() {
    _rootNode->init();
}

shared_ptr<ModelNode> Model::getNodeByNumber(uint16_t number) const {
    return getFromLookupOrNull(_nodeByNumber, number);
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
    for (auto &node : _nodeByNumber) {
        if (node.second->isAABBMesh())
            return node.second;
    }
    return nullptr;
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
