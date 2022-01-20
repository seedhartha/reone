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
#include "../../graphics/textures.h"
#include "../../graphics/window.h"

#include "../graph.h"

#include "camera.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kFadeSpeed = 2.0f;
static constexpr float kMinDirectionalLightRadius = 400.0f;

void LightSceneNode::init() {
    _color = _modelNode->color().getByFrameOrElse(0, glm::vec3(0.0f));
    _radius = _modelNode->radius().getByFrameOrElse(0, 0.0f);
    _multiplier = _modelNode->multiplier().getByFrameOrElse(0, 0.0f);
}

void LightSceneNode::update(float dt) {
    SceneNode::update(dt);

    // Fading
    bool fading = _modelNode->light()->fading;
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
    shared_ptr<Camera> camera(_sceneGraph.camera());
    if (!camera) {
        return;
    }
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetLocals();
    uniforms.general.featureMask = UniformsFeatureFlags::fixedsize;
    uniforms.general.model = glm::translate(getOrigin());
    uniforms.general.billboardSize = 0.2f * flare.size;
    uniforms.general.alpha = 0.5f;
    uniforms.general.color = glm::vec4(_color, 1.0f);

    _shaders.use(_shaders.billboard(), true);
    _textures.bind(*flare.texture);
    _graphicsContext.withBlending(BlendMode::Additive, [this]() {
        _meshes.billboard().draw();
    });
}

bool LightSceneNode::isDirectional() const {
    return _radius >= kMinDirectionalLightRadius;
}

} // namespace scene

} // namespace reone
