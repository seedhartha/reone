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

#include "cubenode.h"

#include <glm/ext.hpp>

#include "../../render/mesh/cube.h"

using namespace reone::render;

namespace reone {

namespace scene {

CubeSceneNode::CubeSceneNode(SceneGraph *sceneGraph, float size) : SceneNode(sceneGraph), _size(size) {
}

void CubeSceneNode::render() const {
    CubeMesh::instance().render(glm::scale(_absoluteTransform, glm::vec3(_size)));
}

} // namespace render

} // namespace reone
