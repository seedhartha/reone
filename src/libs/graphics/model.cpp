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

#include "reone/graphics/model.h"

#include "reone/system/logutil.h"

#include "reone/graphics/animation.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/modelnode.h"
#include "reone/graphics/types.h"

namespace reone {

namespace graphics {

Model::Model(
    std::string name,
    int classification,
    std::shared_ptr<ModelNode> rootNode,
    std::vector<std::shared_ptr<Animation>> animations,
    std::shared_ptr<Model> superModel,
    float animationScale) :
    _name(std::move(name)),
    _classification(classification),
    _rootNode(rootNode),
    _superModel(std::move(superModel)),
    _animationScale(animationScale) {

    if (_rootNode) {
        fillLookups(_rootNode);
    }
    computeAABB();

    for (auto &anim : animations) {
        _animations[anim->name()] = anim;
    }
}

void Model::fillLookups(const std::shared_ptr<ModelNode> &node) {
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

std::shared_ptr<ModelNode> Model::getNodeByNumber(uint16_t number) const {
    auto it = _nodeByNumber.find(number);
    return it != _nodeByNumber.end() ? it->second : nullptr;
}

std::shared_ptr<ModelNode> Model::getNodeByName(const std::string &name) const {
    auto it = _nodeByName.find(name);
    return it != _nodeByName.end() ? it->second : nullptr;
}

std::shared_ptr<ModelNode> Model::getNodeByNameRecursive(const std::string &name) const {
    auto it = _nodeByName.find(name);
    if (it != _nodeByName.end()) {
        return it->second;
    }
    if (_superModel) {
        return _superModel->getNodeByNameRecursive(name);
    } else {
        return nullptr;
    }
}

std::shared_ptr<ModelNode> Model::getAABBNode() const {
    for (auto &node : _nodeByNumber) {
        if (node.second->isAABBMesh())
            return node.second;
    }
    return nullptr;
}

std::vector<std::string> Model::getAnimationNames() const {
    std::vector<std::string> result;

    if (_superModel) {
        std::vector<std::string> superAnims(_superModel->getAnimationNames());
        for (auto &anim : superAnims) {
            result.push_back(anim);
        }
    }
    for (auto &anim : _animations) {
        result.push_back(anim.first);
    }

    return result;
}

std::shared_ptr<Animation> Model::getAnimation(const std::string &name) const {
    auto maybeAnim = _animations.find(name);
    if (maybeAnim != _animations.end())
        return maybeAnim->second;

    std::shared_ptr<Animation> anim;
    if (_superModel) {
        anim = _superModel->getAnimation(name);
    }

    return anim;
}

} // namespace graphics

} // namespace reone
