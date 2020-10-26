/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "aabbnode.h"

#include "../render/mesh/aabb.h"
#include "../system/debug.h"

#include "scenegraph.h"

using namespace reone::render;

namespace reone {

namespace scene {

AABBSceneNode::AABBSceneNode(SceneGraph *sceneGraph, const AABB &aabb) : SceneNode(sceneGraph), _aabb(aabb) {
}

void AABBSceneNode::render() const {
    if (getDebugMode() != DebugMode::ModelNodes) return;

    AABBMesh::instance().render(_aabb, _absoluteTransform);
}

const AABB &AABBSceneNode::aabb() const {
    return _aabb;
}

} // namespace scene

} // namespace reone
