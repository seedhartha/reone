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

#include "../../graphics/shader/shaders.h"

#include "../scenegraph.h"

#include "cameranode.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

MeshSceneNode::MeshSceneNode(SceneGraph *sceneGraph, const shared_ptr<Mesh> &mesh) :
    SceneNode(SceneNodeType::Mesh, sceneGraph),
    _mesh(mesh) {

    if (!mesh) {
        throw invalid_argument("mesh must not be null");
    }
}

void MeshSceneNode::draw() {
    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    uniforms.combined.general.model = _absoluteTransform;
    uniforms.combined.general.color = glm::vec4(_color, 1.0f);
    uniforms.combined.general.alpha = _alpha;
    Shaders::instance().activate(ShaderProgram::ModelColor, uniforms);

    _mesh->draw();

    SceneNode::draw();
}

void MeshSceneNode::setColor(glm::vec3 color) {
    _color = color;
}

void MeshSceneNode::setAlpha(float alpha) {
    _alpha = alpha;
}

} // namespace graphics

} // namespace reone
