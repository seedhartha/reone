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

#include "../../graphics/types.h"

#include "../node.h"

#include "grasscluster.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class Meshes;
class ModelNode;
class Shaders;
class Texture;

} // namespace graphics

namespace scene {

class GrassSceneNode : public SceneNode {
public:
    GrassSceneNode(
        float density,
        float quadSize,
        glm::vec4 probabilities,
        std::set<uint32_t> materials,
        std::shared_ptr<graphics::Texture> texture,
        std::shared_ptr<graphics::ModelNode> aabbNode,
        SceneGraph &sceneGraph,
        graphics::GraphicsContext &graphicsContext,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders) :
        SceneNode(SceneNodeType::Grass, sceneGraph),
        _density(density),
        _quadSize(quadSize),
        _probabilities(std::move(probabilities)),
        _materials(std::move(materials)),
        _texture(std::move(texture)),
        _aabbNode(std::move(aabbNode)),
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders) {

        init();
    }

    void init();

    void update(float dt) override;

    void drawElements(const std::vector<SceneNode *> &elements, int count) override;

    int getNumClustersInFace(float area) const;
    int getRandomGrassVariant() const;

    std::unique_ptr<GrassClusterSceneNode> newCluster();

private:
    float _density;
    float _quadSize;
    glm::vec4 _probabilities;
    std::set<uint32_t> _materials;
    std::shared_ptr<graphics::Texture> _texture;
    std::shared_ptr<graphics::ModelNode> _aabbNode;

    std::vector<int> _grassFaces;
    std::stack<std::shared_ptr<GrassClusterSceneNode>> _clusterPool;                          /**< pre-allocated pool of clusters */
    std::map<int, std::vector<std::shared_ptr<GrassClusterSceneNode>>> _materializedClusters; /**< materialized clusters grouped by face */

    // Services

    graphics::GraphicsContext &_graphicsContext;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;

    // END Services
};

} // namespace scene

} // namespace reone
