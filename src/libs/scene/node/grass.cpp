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

#include "reone/scene/node/grass.h"

#include "reone/graphics/barycentricutil.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/triangleutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/textures.h"
#include "reone/scene/graph.h"
#include "reone/scene/node/grasscluster.h"
#include "reone/scene/render/pipeline.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr int kNumClustersInPool = 2048;
static constexpr float kGrassDensityFactor = 0.25f;

static constexpr float kMaxClusterDistance = 32.0f;
static constexpr float kMaxClusterDistance2 = kMaxClusterDistance * kMaxClusterDistance;

void GrassSceneNode::init() {
    // Compute grass faces
    auto faces = _aabbNode.mesh()->mesh->faces();
    for (size_t faceIdx = 0; faceIdx < faces.size(); ++faceIdx) {
        auto &face = faces[faceIdx];
        if (_properties.materials.count(face.material) == 0) {
            continue;
        }
        _grassFaces.push_back(static_cast<int>(faceIdx));
    }

    // Pre-allocate grass clusters
    for (int i = 0; i < kNumClustersInPool; ++i) {
        _clusterPool.push(_sceneGraph.newGrassCluster(*this).get());
    }
}

void GrassSceneNode::update(float dt) {
    if (!_enabled) {
        return;
    }
    auto camera = _sceneGraph.camera();
    if (!camera) {
        return;
    }
    auto mesh = _aabbNode.mesh()->mesh;
    auto &faces = mesh->faces();
    auto cameraPos = camera->get().origin();
    glm::vec3 meshSpaceCameraPos(_absTransformInv * glm::vec4(cameraPos, 1.0f));

    // Return grass clusters in out-of-distance faces, to the pool
    std::set<int> outOfDistance;
    for (auto &pair : _materializedClusters) {
        auto faceIdx = pair.first;
        auto &face = faces[faceIdx];
        float distance2 = glm::distance2(face.centroid, meshSpaceCameraPos);
        if (distance2 <= kMaxClusterDistance2) {
            continue;
        }
        outOfDistance.insert(faceIdx);
    }
    for (auto &faceIdx : outOfDistance) {
        auto &clusters = _materializedClusters.find(faceIdx)->second;
        for (auto &cluster : clusters) {
            _children.erase(cluster);
            _clusterPool.push(cluster);
        }
        _materializedClusters.erase(faceIdx);
    }

    // Cannot materialize any more grass clusters
    if (_clusterPool.empty()) {
        return;
    }

    // Sort grass faces by distance to camera
    std::multimap<float, int> closestFaces;
    for (size_t faceIdx = 0; faceIdx < faces.size(); ++faceIdx) {
        auto &face = faces[faceIdx];
        if (_properties.materials.count(face.material) == 0) {
            continue;
        }
        float distance2 = glm::distance2(face.centroid, meshSpaceCameraPos);
        if (distance2 > kMaxClusterDistance2) {
            continue;
        }
        closestFaces.insert(std::make_pair(distance2, static_cast<int>(faceIdx)));
    }

    // Materialize grass clusters in closest faces, from the pool
    for (auto &pair : closestFaces) {
        auto faceIdx = pair.second;
        if (_materializedClusters.count(faceIdx) > 0) {
            continue;
        }
        auto &face = faces[faceIdx];
        auto verts = mesh->faceVertexCoords(face);
        for (int i = 0; i < getNumClustersInFace(face.area); ++i) {
            if (_clusterPool.empty()) {
                return;
            }
            glm::vec3 baryPosition(getRandomBarycentric());
            glm::vec3 position(barycentricToCartesian(verts[0], verts[1], verts[2], baryPosition));
            glm::vec2 lightmapUV(mesh->faceUV2(face, baryPosition));
            auto cluster = _clusterPool.top();
            _clusterPool.pop();
            cluster->setLocalTransform(glm::translate(position));
            cluster->setVariant(getRandomGrassVariant());
            cluster->setLightmapUV(std::move(lightmapUV));
            addChild(*cluster);
            _materializedClusters[faceIdx].push_back(cluster);
        }
    }
}

void GrassSceneNode::renderLeafs(IRenderPass &pass, const std::vector<SceneNode *> &leafs) {
    if (leafs.empty()) {
        return;
    }
    std::optional<std::reference_wrapper<Texture>> lightmap;
    if (!_aabbNode.mesh()->lightmap.empty()) {
        lightmap = *_resourceSvc.textures.get(_aabbNode.mesh()->lightmap, TextureUsage::Lightmap);
    }
    auto instances = std::vector<GrassInstance>(leafs.size());
    for (size_t i = 0; i < leafs.size(); ++i) {
        const auto cluster = static_cast<GrassClusterSceneNode *>(leafs[i]);
        instances[i].position = cluster->origin();
        instances[i].variant = cluster->variant();
        instances[i].lightmapUV = cluster->lightmapUV();
    }
    pass.drawGrass(kMaxClusterDistance,
                   _properties.quadSize,
                   *_properties.texture,
                   lightmap,
                   instances);
}

int GrassSceneNode::getNumClustersInFace(float area) const {
    return static_cast<int>(glm::round(kGrassDensityFactor * _properties.density * area));
}

int GrassSceneNode::getRandomGrassVariant() const {
    float sum = _properties.probabilities[0] + _properties.probabilities[1] + _properties.probabilities[2] + _properties.probabilities[3];
    float val = randomFloat(0.0f, 1.0f) * sum;
    float upper = 0.0f;
    for (int i = 0; i < 3; ++i) {
        upper += _properties.probabilities[i];
        if (val < upper) {
            return i;
        }
    }
    return 3;
}

} // namespace scene

} // namespace reone
