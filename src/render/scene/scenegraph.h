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

#pragma once

#include <memory>
#include <vector>

#include "glm/vec3.hpp"

#include "aabbnode.h"
#include "meshnode.h"

namespace reone {

namespace render {

class SceneGraph {
public:
    SceneGraph() = default;

    void clear();
    void add(const std::shared_ptr<MeshSceneNode> &node);
    void add(const std::shared_ptr<AABBSceneNode> &node);
    void prepare(const glm::vec3 &cameraPosition);
    void render() const;

private:
    std::vector<std::shared_ptr<MeshSceneNode>> _opaqueMeshes;
    std::vector<std::shared_ptr<MeshSceneNode>> _transparentMeshes;
    std::vector<std::shared_ptr<AABBSceneNode>> _aabbNodes;

    SceneGraph(const SceneGraph &) = delete;
    SceneGraph &operator=(const SceneGraph &) = delete;
};

} // namespace render

} // namespace reone
