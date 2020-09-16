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

#include "meshnode.h"

using namespace std;

namespace reone {

namespace render {

MeshSceneNode::MeshSceneNode(const ModelSceneNode *model, const ModelNode *modelNode, const glm::mat4 &transform) :
    SceneNode(transform), _model(model), _modelNode(modelNode) {

    assert(_model && _modelNode);
    _origin = glm::vec4(_modelNode->mesh()->aabb().center(), 1.0f) * transform;
}

bool MeshSceneNode::isTransparent() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    return (mesh && mesh->isTransparent()) || _modelNode->alpha() < 1.0f;
}

const ModelSceneNode *MeshSceneNode::model() const {
    return _model;
}

const ModelNode *MeshSceneNode::modelNode() const {
    return _modelNode;
}

const glm::vec3 &MeshSceneNode::origin() const {
    return _origin;
}

} // namespace render

} // namespace reone
