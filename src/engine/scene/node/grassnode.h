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

#include "../../graphics/texture/texture.h"

#include "../grasscluster.h"

#include "scenenode.h"

namespace reone {

namespace scene {

class CameraSceneNode;

class GrassSceneNode : public SceneNode {
public:
    GrassSceneNode(SceneGraph *graph, glm::vec2 quadSize, std::shared_ptr<graphics::Texture> texture, std::shared_ptr<graphics::Texture> lightmap = nullptr);

    void clear();
    void addCluster(GrassCluster cluster);

    void drawClusters(const std::vector<GrassCluster> &clusters);

    const std::vector<GrassCluster> &clusters() const { return _clusters; }

private:
    glm::vec2 _quadSize { 0.0f };
    std::shared_ptr<graphics::Texture> _texture;
    std::shared_ptr<graphics::Texture> _lightmap;
    std::vector<GrassCluster> _clusters;
};

} // namespace scene

} // namespace reone
