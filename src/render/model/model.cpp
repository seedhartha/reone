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

namespace render {

Model::Model(
    string name,
    Classification classification,
    float animationScale,
    shared_ptr<ModelNode> rootNode,
    vector<shared_ptr<Animation>> animations,
    shared_ptr<Model> superModel
) :
    _name(move(name)),
    _classification(classification),
    _animationScale(animationScale),
    _rootNode(rootNode),
    _superModel(move(superModel)) {

    if (!rootNode) {
        throw invalid_argument("rootNode must not be null");
    }
    for (auto &anim : animations) {
        _animations.insert(make_pair(anim->name(), move(anim)));
    }
    initInternal(_rootNode);
}

void Model::initInternal(const shared_ptr<ModelNode> &node) {
    _nodeByNumber.insert(make_pair(node->nodeNumber(), node));
    _nodeByName.insert(make_pair(node->name(), node));

    shared_ptr<ModelMesh> mesh(node->mesh());
    if (mesh) {
        _aabb.expand(mesh->mesh()->aabb() * node->absoluteTransform());
    }

    for (auto &child : node->children()) {
        initInternal(child);
    }
}

void Model::init() {
    _rootNode->init();
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
    if (maybeAnim != _animations.end()) return maybeAnim->second;

    shared_ptr<Animation> anim;
    if (_superModel) {
        anim = _superModel->getAnimation(name);
    }
    if (!anim) {
        debug(boost::format("Model animation not found: '%s' '%s'") % name % _name, 2);
    }

    return move(anim);
}

shared_ptr<ModelNode> Model::findNodeByNumber(uint16_t number) const {
    return getFromLookupOrNull(_nodeByNumber, number);
}

shared_ptr<ModelNode> Model::findNodeByName(const string &name) const {
    return getFromLookupOrNull(_nodeByName, name);
}

shared_ptr<ModelNode> Model::findAABBNode() const {
    for (auto &node : _nodeByNumber) {
        if (node.second->isAABB()) return node.second;
    }
    return nullptr;
}

} // namespace render

} // namespace reone
