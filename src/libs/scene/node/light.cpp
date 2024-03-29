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
#include "reone/graphics/material.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniforms.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/textures.h"
#include "reone/scene/graph.h"
#include "reone/scene/node/camera.h"
#include "reone/scene/render/pipeline.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kFadeSpeed = 2.0f;
static constexpr float kMinDirectionalLightRadius = 100.0f;

void LightSceneNode::init() {
    _modelNode.vectorValueAtTime(ControllerTypes::color, 0.0f, _color);
    _modelNode.floatValueAtTime(ControllerTypes::radius, 0.0f, _radius);
    _modelNode.floatValueAtTime(ControllerTypes::multiplier, 0.0f, _multiplier);
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

void LightSceneNode::renderLensFlare(IRenderPass &pass, const ModelNode::LensFlare &flare) {
    std::shared_ptr<Camera> camera(_sceneGraph.camera()->get().camera());
    if (!camera) {
        return;
    }
    auto texture = _resourceSvc.textures.get(flare.textureName);
    if (!texture) {
        return;
    }
    auto color = glm::vec4(_color, 0.5f);
    auto transform = glm::translate(origin());
    pass.drawBillboard(*texture, color, transform, glm::inverse(transform), 0.2f * flare.size);
}

bool LightSceneNode::isDirectional() const {
    return _radius >= kMinDirectionalLightRadius;
}

} // namespace scene

} // namespace reone
