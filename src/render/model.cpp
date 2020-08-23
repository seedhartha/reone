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

#include "model.h"

using namespace std;

namespace reone {

namespace render {

Model::Model(
    const string &name,
    const shared_ptr<ModelNode> &rootNode,
    const vector<shared_ptr<Animation>> &anims,
    const shared_ptr<Model> &superModel
) :
    _name(name),
    _rootNode(rootNode),
    _superModel(superModel) {

    for (auto &anim : anims) {
        _animations.insert(make_pair(anim->name(), anim));
    }

    init(_rootNode);

    glm::vec3 aabbSize(_aabb.size());
    _radiusXY = 0.5f * glm::max(aabbSize.x, aabbSize.y);
}

void Model::init(const shared_ptr<ModelNode> &node) {
    _nodeByNumber.insert(make_pair(node->nodeNumber(), node));
    _nodeByName.insert(make_pair(node->name(), node));

    shared_ptr<ModelMesh> mesh(node->mesh());
    if (mesh) {
        _aabb.expand(mesh->aabb() * node->absoluteTransform());
    }

    for (auto &child : node->children()) {
        init(child);
    }
}

void Model::initGL() {
    _rootNode->initGL();
}

shared_ptr<Animation> Model::findAnimation(const string &name, const Model **model) const {
    auto it = _animations.find(name);
    if (it != _animations.end()) {
        *model = this;
        return it->second;
    }
    if (_superModel) {
        return _superModel->findAnimation(name, model);
    }

    return nullptr;
}

shared_ptr<ModelNode> Model::findNodeByNumber(uint16_t number) const {
    auto it = _nodeByNumber.find(number);
    return it != _nodeByNumber.end() ? it->second : nullptr;
}

shared_ptr<ModelNode> Model::findNodeByName(const string &name) const {
    auto it = _nodeByName.find(name);
    return it != _nodeByName.end() ? it->second : nullptr;
}

const string &Model::name() const {
    return _name;
}

const ModelNode &Model::rootNode() const {
    return *_rootNode;
}

float Model::animationScale() const {
    return _animationScale;
}

shared_ptr<Model> Model::superModel() const {
    return _superModel;
}

const AABB &Model::aabb() const {
    return _aabb;
}

float Model::radiusXY() const {
    return _radiusXY;
}

void Model::setAnimationScale(float scale) {
    _animationScale = scale;
}

} // namespace render

} // namespace reone
