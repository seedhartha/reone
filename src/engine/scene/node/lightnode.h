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

#pragma once

#include "../../graphics/model/modelnode.h"

#include "modelnodescenenode.h"

namespace reone {

namespace scene {

class ModelSceneNode;

class LightSceneNode : public ModelNodeSceneNode {
public:
    LightSceneNode(const ModelSceneNode *model, std::shared_ptr<graphics::ModelNode> modelNode, SceneGraph *sceneGraph);

    void update(float dt) override;
    void drawLensFlares(const graphics::ModelNode::LensFlare &flare);

    bool isActive() const { return _active; }
    bool isDirectional() const;

    const glm::vec3 &color() const { return _color; }
    float radius() const { return _radius; }
    float multiplier() const { return _multiplier; }
    float fadeFactor() const { return _fadeFactor; }

    void setColor(glm::vec3 color) { _color = std::move(color); }
    void setRadius(float radius) { _radius = radius; }
    void setMultiplier(float multiplier) { _multiplier = multiplier; }
    void setActive(bool active) { _active = active; }
    void setFadeFactor(float factor) { _fadeFactor = factor; }

private:
    const ModelSceneNode *_model;

    glm::vec3 _color { 0.0f };
    float _radius { 0.0f };
    float _multiplier { 0.0f };

    // Fading

    bool _active { false }; /**< fade in if true, fade out otherwise */
    float _fadeFactor { 0.0f };

    // END Fading
};

} // namespace scene

} // namespace reone
