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

#include "../../common/guardutil.h"
#include "../../di/services/graphics.h"
#include "../../graphics/context.h"
#include "../../graphics/mesh/mesh.h"
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/texture/texture.h"
#include "../../graphics/window.h"

#include "../scenegraph.h"

#include "camera.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kFadeSpeed = 1.0f;
static constexpr float kMinDirectionalLightRadius = 1000.0f;

LightSceneNode::LightSceneNode(const ModelSceneNode *model, shared_ptr<ModelNode> modelNode, SceneGraph *sceneGraph) :
    ModelNodeSceneNode(modelNode, SceneNodeType::Light, sceneGraph),
    _model(model) {

    ensurePresent(model, "model");

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
    shared_ptr<CameraSceneNode> camera(_sceneGraph->activeCamera());
    if (!camera) return;

    _sceneGraph->graphics().context().setActiveTextureUnit(TextureUnits::diffuseMap);
    flare.texture->bind();

    glm::vec4 lightPos(_absTransform[3]);
    glm::vec4 lightPosNdc(camera->projection() * camera->view() * lightPos);

    float w = static_cast<float>(_sceneGraph->options().width);
    float h = static_cast<float>(_sceneGraph->options().height);

    glm::vec3 lightPosScreen(glm::vec3(lightPosNdc) / lightPosNdc.w);
    lightPosScreen *= 0.5f;
    lightPosScreen += 0.5f;
    lightPosScreen *= glm::vec3(w, h, 1.0f);

    float aspect = flare.texture->width() / static_cast<float>(flare.texture->height());
    float baseFlareSize = 50.0f;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(lightPosScreen.x, lightPosScreen.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(aspect * flare.size * baseFlareSize, flare.size * baseFlareSize, 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = glm::ortho(0.0f, w, 0.0f, h);
    uniforms.combined.general.model = move(transform);
    uniforms.combined.general.alpha = 0.5f;
    //uniforms.combined.general.color = glm::vec4(flare.colorShift, 1.0f);

    _sceneGraph->graphics().shaders().activate(ShaderProgram::SimpleGUI, uniforms);

    BlendMode oldBlendMode = _sceneGraph->graphics().context().blendMode();
    _sceneGraph->graphics().context().setBlendMode(BlendMode::Add);
    _sceneGraph->graphics().meshes().billboard().draw();
    _sceneGraph->graphics().context().setBlendMode(oldBlendMode);
}

bool LightSceneNode::isDirectional() const {
    return _radius >= kMinDirectionalLightRadius;
}

} // namespace scene

} // namespace reone
