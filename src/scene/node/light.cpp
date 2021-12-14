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

#include "light.h"

#include "../../graphics/context.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"
#include "../../graphics/window.h"

#include "../graph.h"

#include "camera.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kFadeSpeed = 1.0f;
static constexpr float kMinDirectionalLightRadius = 1000.0f;

LightSceneNode::LightSceneNode(
    shared_ptr<ModelNode> modelNode,
    SceneGraph &sceneGraph,
    Context &context,
    Meshes &meshes,
    Shaders &shaders) :
    ModelNodeSceneNode(
        modelNode,
        SceneNodeType::Light,
        sceneGraph,
        context,
        meshes,
        shaders) {

    _color = modelNode->color().getByFrameOrElse(0, glm::vec3(0.0f));
    _radius = modelNode->radius().getByFrameOrElse(0, 0.0f);
    _multiplier = modelNode->multiplier().getByFrameOrElse(0, 0.0f);
}

void LightSceneNode::update(float dt) {
    SceneNode::update(dt);

    if (_active) {
        // Fade out
        _fadeFactor = _modelNode->light()->fading ? glm::max(0.0f, _fadeFactor - kFadeSpeed * dt) : 0.0f;
    } else {
        // Fade in
        _fadeFactor = _modelNode->light()->fading ? glm::min(1.0f, _fadeFactor + kFadeSpeed * dt) : 1.0f;
    }
}

void LightSceneNode::drawLensFlares(const ModelNode::LensFlare &flare) {
    shared_ptr<CameraSceneNode> camera(_sceneGraph.activeCamera());
    if (!camera)
        return;

    _context.bindTexture(TextureUnits::diffuseMap, flare.texture);

    glm::vec4 lightPos(_absTransform[3]);
    glm::vec4 lightPosNdc(camera->projection() * camera->view() * lightPos);

    float w = static_cast<float>(_sceneGraph.options().width);
    float h = static_cast<float>(_sceneGraph.options().height);

    glm::vec3 lightPosScreen(glm::vec3(lightPosNdc) / lightPosNdc.w);
    lightPosScreen *= 0.5f;
    lightPosScreen += 0.5f;
    lightPosScreen *= glm::vec3(w, h, 1.0f);

    float aspect = flare.texture->width() / static_cast<float>(flare.texture->height());
    float baseFlareSize = 50.0f;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(lightPosScreen.x, lightPosScreen.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(aspect * flare.size * baseFlareSize, flare.size * baseFlareSize, 1.0f));

    auto &uniforms = _shaders.uniforms();
    uniforms.general = GeneralUniforms();
    uniforms.general.projection = glm::ortho(0.0f, w, 0.0f, h);
    uniforms.general.model = move(transform);
    uniforms.general.alpha = 0.5f;
    // uniforms.general.color = glm::vec4(flare.colorShift, 1.0f);

    BlendMode oldBlendMode = _context.blendMode();
    _context.setBlendMode(BlendMode::Add);

    _context.useShaderProgram(_shaders.gui());
    _shaders.refreshUniforms();
    _meshes.billboard().draw();

    _context.setBlendMode(oldBlendMode);
}

bool LightSceneNode::isDirectional() const {
    return _radius >= kMinDirectionalLightRadius;
}

} // namespace scene

} // namespace reone
