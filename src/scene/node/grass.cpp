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

#include "grass.h"

#include "../../graphics/barycentricutil.h"
#include "../../graphics/context.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"
#include "../../graphics/textures.h"
#include "../../graphics/triangleutil.h"

#include "../graph.h"

#include "grasscluster.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr int kNumClustersInPool = 2048;
static constexpr float kGrassDensityFactor = 0.25f;

static constexpr float kMaxClusterDistance = 32.0f;
static constexpr float kMaxClusterDistance2 = kMaxClusterDistance * kMaxClusterDistance;

void GrassSceneNode::init() {
    // Compute grass faces
    auto faces = _aabbNode->mesh()->mesh->faces();
    for (size_t faceIdx = 0; faceIdx < faces.size(); ++faceIdx) {
        auto &face = faces[faceIdx];
        if (_materials.count(face.material) == 0) {
            continue;
        }
        _grassFaces.push_back(static_cast<int>(faceIdx));
    }

    // Pre-allocate grass clusters
    for (int i = 0; i < kNumClustersInPool; ++i) {
        _clusterPool.push(newCluster());
    }
}

void GrassSceneNode::update(float dt) {
    if (!_enabled) {
        return;
    }
    auto camera = _sceneGraph.activeCamera();
    if (!camera) {
        return;
    }
    auto mesh = _aabbNode->mesh()->mesh;
    auto &faces = mesh->faces();
    auto cameraPos = camera->getOrigin();
    glm::vec3 meshSpaceCameraPos(_absTransformInv * glm::vec4(cameraPos, 1.0f));

    // Return grass clusters in out-of-distance faces, to the pool
    set<int> outOfDistance;
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
    multimap<float, int> closestFaces;
    for (size_t faceIdx = 0; faceIdx < faces.size(); ++faceIdx) {
        auto &face = faces[faceIdx];
        if (_materials.count(face.material) == 0) {
            continue;
        }
        float distance2 = glm::distance2(face.centroid, meshSpaceCameraPos);
        if (distance2 > kMaxClusterDistance2) {
            continue;
        }
        closestFaces.insert(make_pair(distance2, faceIdx));
    }

    // Materialize grass clusters in closest faces, from the pool
    for (auto &pair : closestFaces) {
        auto faceIdx = pair.second;
        if (_materializedClusters.count(faceIdx) > 0) {
            continue;
        }
        auto &face = faces[faceIdx];
        auto verts = mesh->getVertexCoords(face);
        for (int i = 0; i < getNumClustersInFace(face.area); ++i) {
            if (_clusterPool.empty()) {
                return;
            }
            glm::vec3 baryPosition(getRandomBarycentric());
            glm::vec3 position(barycentricToCartesian(verts[0], verts[1], verts[2], baryPosition));
            glm::vec2 lightmapUV(mesh->getUV2(face, baryPosition));
            auto cluster = _clusterPool.top();
            _clusterPool.pop();
            cluster->setLocalTransform(glm::translate(position));
            cluster->setVariant(getRandomGrassVariant());
            cluster->setLightmapUV(move(lightmapUV));
            addChild(cluster);
            _materializedClusters[faceIdx].push_back(move(cluster));
        }
    }
}

void GrassSceneNode::drawLeafs(const vector<SceneNode *> &leafs) {
    if (leafs.empty()) {
        return;
    }
    _textures.bind(*_texture);

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetLocals();
    uniforms.general.featureMask = UniformsFeatureFlags::hashedalphatest;
    if (_aabbNode->mesh()->lightmap) {
        _textures.bind(*_aabbNode->mesh()->lightmap, TextureUnits::lightmap);
        uniforms.general.featureMask |= UniformsFeatureFlags::lightmap;
    }
    for (size_t i = 0; i < leafs.size(); ++i) {
        auto cluster = static_cast<GrassClusterSceneNode *>(leafs[i]);
        uniforms.grass.quadSize = glm::vec2(_quadSize);
        uniforms.grass.radius = kMaxClusterDistance;
        uniforms.grass.clusters[i].positionVariant = glm::vec4(cluster->getOrigin(), static_cast<float>(cluster->variant()));
        uniforms.grass.clusters[i].lightmapUV = cluster->lightmapUV();
    }

    _shaders.use(_shaders.grass(), true);
    _shaders.refreshGrassUniforms();
    _meshes.grass().drawInstanced(leafs.size());
}

int GrassSceneNode::getNumClustersInFace(float area) const {
    return static_cast<int>(glm::round(kGrassDensityFactor * _density * area));
}

int GrassSceneNode::getRandomGrassVariant() const {
    float sum = _probabilities[0] + _probabilities[1] + _probabilities[2] + _probabilities[3];
    float val = random(0.0f, 1.0f) * sum;
    float upper = 0.0f;
    for (int i = 0; i < 3; ++i) {
        upper += _probabilities[i];
        if (val < upper) {
            return i;
        }
    }
    return 3;
}

unique_ptr<GrassClusterSceneNode> GrassSceneNode::newCluster() {
    return make_unique<GrassClusterSceneNode>(_sceneGraph);
}

} // namespace scene

} // namespace reone
