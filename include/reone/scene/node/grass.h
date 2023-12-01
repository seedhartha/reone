/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/modelnode.h"
#include "reone/graphics/types.h"

#include "../grassproperties.h"
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
        GrassProperties properties,
        graphics::ModelNode &aabbNode,
        SceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc) :
        SceneNode(
            SceneNodeType::Grass,
            sceneGraph,
            graphicsSvc,
            audioSvc,
            resourceSvc),
        _properties(std::move(properties)),
        _aabbNode(aabbNode) {

        init();
    }

    void init();

    void update(float dt) override;

    void drawLeafs(const std::vector<SceneNode *> &leafs) override;

    int getNumClustersInFace(float area) const;
    int getRandomGrassVariant() const;

private:
    GrassProperties _properties;
    graphics::ModelNode &_aabbNode;

    std::vector<int> _grassFaces;
    std::stack<GrassClusterSceneNode *> _clusterPool;                          /**< pre-allocated pool of clusters */
    std::map<int, std::vector<GrassClusterSceneNode *>> _materializedClusters; /**< materialized clusters grouped by face */
};

} // namespace scene

} // namespace reone
