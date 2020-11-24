/*
 * Copyright (c) 2020 The reone project contributors
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

#include "scenegraph.h"

namespace reone {

namespace scene {

LightSceneNode::LightSceneNode(SceneGraph *sceneGraph, int priority, const glm::vec3 &color, float radius, float multiplier, bool shadow) :
    SceneNode(sceneGraph),
    _priority(priority),
    _color(color),
    _radius(radius),
    _multiplier(multiplier),
    _shadow(shadow) {
}

int LightSceneNode::priority() const {
    return _priority;
}

const glm::vec3 &LightSceneNode::color() const {
    return _color;
}

float LightSceneNode::radius() const {
    return _radius;
}

float LightSceneNode::multiplier() const {
    return _multiplier;
}

bool LightSceneNode::shadow() const {
    return _shadow;
}

} // namespace scene

} // namespace reone
