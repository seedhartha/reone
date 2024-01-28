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

#pragma once

#include "modelnode.h"

namespace reone {

namespace scene {

class ModelSceneNode;

class LightSceneNode : public ModelNodeSceneNode {
public:
    LightSceneNode(
        ModelSceneNode &model,
        graphics::ModelNode &modelNode,
        ISceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc) :
        ModelNodeSceneNode(
            modelNode,
            SceneNodeType::Light,
            sceneGraph,
            graphicsSvc,
            audioSvc,
            resourceSvc),
        _model(model) {

        init();
    }

    void init();

    void update(float dt) override;

    void renderLensFlare(IRenderPass &pass, const graphics::ModelNode::LensFlare &flare);

    bool isDirectional() const;

    const ModelSceneNode &model() const { return _model; }
    const glm::vec3 &color() const { return _color; }
    float radius() const { return _radius; }
    float multiplier() const { return _multiplier; }

    void setColor(glm::vec3 color) { _color = std::move(color); }
    void setRadius(float radius) { _radius = radius; }
    void setMultiplier(float multiplier) { _multiplier = multiplier; }

    // Fading

    bool isActive() const { return _active; }

    void setActive(bool active) { _active = active; }
    void setStrength(float strength) { _strength = strength; }

    float strength() const { return _strength; }

    // END Fading

private:
    ModelSceneNode &_model;

    glm::vec3 _color {0.0f};
    float _radius {0.0f};
    float _multiplier {0.0f};

    // Fading

    bool _active {false};
    float _strength {0.0f};

    // END Fading
};

} // namespace scene

} // namespace reone
