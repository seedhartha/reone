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

#include "scenenode.h"

namespace reone {

namespace scene {

class LightSceneNode : public SceneNode {
public:
    LightSceneNode(SceneGraph *sceneGraph, int priority, const glm::vec3 &color, float radius, float multiplier, bool shadow);

    int priority() const { return _priority; }
    const glm::vec3 &color() const { return _color; }
    float radius() const { return _radius; }
    float multiplier() const { return _multiplier; }
    bool shadow() const { return _shadow; }

private:
    int _priority { 0 };
    glm::vec3 _color { 1.0f };
    float _radius { 1.0f };
    float _multiplier { 1.0f };
    bool _shadow { false };
};

} // namespace scene

} // namespace reone
