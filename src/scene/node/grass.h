/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../../graphics/modelnode.h"
#include "../../graphics/types.h"

#include "../node.h"

#include "grasscluster.h"

namespace reone {

namespace graphics {

struct GraphicsServices;

}

namespace scene {

class GrassSceneNode : public SceneNode {
public:
    GrassSceneNode(
        float density,
        float quadSize,
        glm::vec4 probabilities,
        std::set<uint32_t> materials,
        graphics::Texture &texture,
        graphics::ModelNode &aabbNode,
        SceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc) :
        SceneNode(
            SceneNodeType::Grass,
            sceneGraph,
            graphicsSvc,
            audioSvc),
        _density(density),
        _quadSize(quadSize),
        _probabilities(std::move(probabilities)),
        _materials(std::move(materials)),
        _texture(texture),
        _aabbNode(aabbNode) {

        init();
    }

    void init();

    void update(float dt) override;

    void drawLeafs(const std::vector<SceneNode *> &leafs) override;

    int getNumClustersInFace(float area) const;
    int getRandomGrassVariant() const;

private:
    float _density;
    float _quadSize;
    glm::vec4 _probabilities;
    std::set<uint32_t> _materials;
    graphics::Texture &_texture;
    graphics::ModelNode &_aabbNode;

    std::vector<int> _grassFaces;
    std::stack<GrassClusterSceneNode *> _clusterPool;                          /**< pre-allocated pool of clusters */
    std::map<int, std::vector<GrassClusterSceneNode *>> _materializedClusters; /**< materialized clusters grouped by face */
};

} // namespace scene

} // namespace reone
