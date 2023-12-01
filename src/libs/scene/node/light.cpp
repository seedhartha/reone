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

#include "reone/scene/node/light.h"

#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshes.h"
#include "reone/graphics/shaders.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/resource/textures.h"

#include "reone/scene/graph.h"

#include "reone/scene/node/camera.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kFadeSpeed = 2.0f;
static constexpr float kMinDirectionalLightRadius = 100.0f;

void LightSceneNode::init() {
    _color = _modelNode.color().getByFrameOrElse(0, glm::vec3(0.0f));
    _radius = _modelNode.radius().getByFrameOrElse(0, 0.0f);
    _multiplier = _modelNode.multiplier().getByFrameOrElse(0, 0.0f);
}

void LightSceneNode::update(float dt) {
    SceneNode::update(dt);

    // Fading
    bool fading = _modelNode.light()->fading;
    if (_active) {
        if (fading) {
            _strength = glm::min(1.0f, _strength + kFadeSpeed * dt);
        } else {
            _strength = 1.0f;
        }
    } else {
        if (fading) {
            _strength = glm::max(0.0f, _strength - kFadeSpeed * dt);
        } else {
            _strength = 0.0f;
        }
    }
}

void LightSceneNode::drawLensFlare(const ModelNode::LensFlare &flare) {
    std::shared_ptr<Camera> camera(_sceneGraph.camera());
    if (!camera) {
        return;
    }
    _graphicsSvc.uniforms.setGeneral([this, &flare](auto &general) {
        general.resetLocals();
        general.featureMask = UniformsFeatureFlags::fixedsize;
        general.model = glm::translate(getOrigin());
        general.billboardSize = 0.2f * flare.size;
        general.alpha = 0.5f;
        general.color = glm::vec4(_color, 1.0f);
    });
    _graphicsSvc.shaders.use(ShaderProgramId::Billboard);
    _graphicsSvc.context.bind(*flare.texture);
    _graphicsSvc.context.withBlending(BlendMode::Additive, [this]() {
        _graphicsSvc.meshes.billboard().draw();
    });
}

bool LightSceneNode::isDirectional() const {
    return _radius >= kMinDirectionalLightRadius;
}

} // namespace scene

} // namespace reone
