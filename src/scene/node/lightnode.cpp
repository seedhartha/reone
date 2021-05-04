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

#include "lightnode.h"

#include "glm/ext.hpp"

#include "../../render/meshes.h"
#include "../../render/shaders.h"
#include "../../render/stateutil.h"
#include "../../render/window.h"

#include "../scenegraph.h"

#include "cameranode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

LightSceneNode::LightSceneNode(int priority, SceneGraph *sceneGraph) :
    SceneNode(SceneNodeType::Light, sceneGraph),
    _priority(priority) {
}

void LightSceneNode::drawLensFlares(const LensFlare &flare) {
    shared_ptr<CameraSceneNode> camera(_sceneGraph->activeCamera());
    if (!camera) return;

    setActiveTextureUnit(0);
    flare.texture->bind();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_absoluteTransform[3]));
    transform = glm::scale(transform, glm::vec3(flare.texture->height()));

    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    uniforms.combined.general.model = move(transform);
    uniforms.combined.general.color = glm::vec4(flare.colorShift, 1.0f);

    Shaders::instance().activate(ShaderProgram::BillboardGUI, uniforms);

    withAdditiveBlending([]() {
        Meshes::instance().getBillboard()->draw();
    });
}

} // namespace scene

} // namespace reone
