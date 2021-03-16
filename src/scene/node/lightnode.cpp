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

#include "lightnode.h"

#include "../scenegraph.h"

using namespace std;

namespace reone {

namespace scene {

LightSceneNode::LightSceneNode(glm::vec3 color, int priority, SceneGraph *sceneGraph) :
    SceneNode(SceneNodeType::Light, sceneGraph),
    _color(move(color)),
    _priority(priority) {
}

void LightSceneNode::setMultiplier(float multiplier) {
    _multiplier = multiplier;
}

void LightSceneNode::setRadius(float radius) {
    _radius = radius;
}

void LightSceneNode::setShadow(bool shadow) {
    _shadow = shadow;
}

void LightSceneNode::setAmbientOnly(bool ambientOnly) {
    _ambientOnly = ambientOnly;
}

} // namespace scene

} // namespace reone
