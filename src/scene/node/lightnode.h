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

#include "../types.h"

#include "../../graphics/model/lensflare.h"

#include "scenenode.h"

namespace reone {

namespace scene {

class LightSceneNode : public SceneNode {
public:
    LightSceneNode(int priority, SceneGraph *sceneGraph);

    void drawLensFlares(const graphics::LensFlare &flare);

    bool isShadow() const { return _shadow; }
    bool isAmbientOnly() const { return _ambientOnly; }
    bool isDirectional() const { return _directional; }

    const glm::vec3 &color() const { return _color; }
    int priority() const { return _priority; }
    float multiplier() const { return _multiplier; }
    float radius() const { return _radius; }
    float flareRadius() const { return _flareRadius; }
    const std::vector<graphics::LensFlare> &flares() const { return _flares; }

    void setColor(glm::vec3 color) { _color = std::move(color); }
    void setMultiplier(float multiplier) { _multiplier = multiplier; }
    void setRadius(float radius) { _radius = radius; }
    void setShadow(bool shadow) { _shadow = shadow; }
    void setAmbientOnly(bool ambientOnly) { _ambientOnly = ambientOnly; }
    void setDirectional(bool directional) { _directional = directional; }
    void setFlareRadius(float radius) { _flareRadius = radius; }
    void setFlares(std::vector<graphics::LensFlare> flares) { _flares = std::move(flares); }

private:
    int _priority;

    glm::vec3 _color { 1.0f };
    float _multiplier { 1.0f };
    float _radius { 1.0f };
    bool _shadow { false };
    bool _ambientOnly { false };
    bool _directional { false };

    // Light flares

    float _flareRadius { 0.0f };
    std::vector<graphics::LensFlare> _flares;

    // END Light flares
};

} // namespace scene

} // namespace reone
