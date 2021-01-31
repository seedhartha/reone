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

#include "meshnode.h"

#include <stdexcept>

#include "glm/ext.hpp"

#include "../../render/shaders.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

MeshSceneNode::MeshSceneNode(SceneGraph *sceneGraph, const shared_ptr<Mesh> &mesh) : SceneNode(sceneGraph), _mesh(mesh) {
    if (!mesh) {
        throw invalid_argument("mesh must not be null");
    }
}

void MeshSceneNode::render() const {
    LocalUniforms locals;
    locals.general.model = _absoluteTransform;
    locals.general.color = glm::vec4(_color, 1.0f);
    locals.general.alpha = _alpha;

    Shaders::instance().activate(ShaderProgram::ModelWhite, locals);

    _mesh->renderTriangles();

    SceneNode::render();
}

void MeshSceneNode::setColor(glm::vec3 color) {
    _color = color;
}

void MeshSceneNode::setAlpha(float alpha) {
    _alpha = alpha;
}

} // namespace render

} // namespace reone
