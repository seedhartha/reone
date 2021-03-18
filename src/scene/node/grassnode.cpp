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

#include "grassnode.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_map>

#include "glm/gtx/norm.hpp"

#include "../../render/meshes.h"
#include "../../render/shaders.h"
#include "../../render/stateutil.h"

#include "../scenegraph.h"

#include "cameranode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

GrassSceneNode::GrassSceneNode(SceneGraph *graph, shared_ptr<Texture> texture, glm::vec2 quadSize) :
    SceneNode(SceneNodeType::Grass, graph),
    _texture(texture),
    _quadSize(move(quadSize)) {

    if (!texture) {
        throw invalid_argument("texture must not be null");
    }
    _transparent = true;
}

void GrassSceneNode::clear() {
    _clusters.clear();
}

void GrassSceneNode::addCluster(GrassCluster cluster) {
    _clusters.push_back(move(cluster));
}

void GrassSceneNode::sortClustersBackToFront(const CameraSceneNode &camera) {
    unordered_map<GrassCluster *, float> clusterZ;
    for (auto &cluster : _clusters) {
        glm::vec4 screenCoords(camera.projection() * camera.view() * glm::vec4(cluster.position, 1.0f));
        screenCoords /= screenCoords.w;
        clusterZ.insert(make_pair(&cluster, screenCoords.z));
    }
    sort(_clusters.begin(), _clusters.end(), [&](auto &left, auto &right) {
        return clusterZ.find(&left)->second > clusterZ.find(&right)->second;
    });
}

void GrassSceneNode::drawSingle() {
    if (_clusters.empty()) return;

    setActiveTextureUnit(TextureUnits::diffuse);
    _texture->bind();

    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    uniforms.general.featureMask |= UniformFeatureFlags::grass;
    for (size_t i = 0; i < _clusters.size(); ++i) {
        uniforms.grass.quadSize = _quadSize;
        uniforms.grass.clusters[i].positionVariant = glm::vec4(_clusters[i].position, static_cast<float>(_clusters[i].variant));
    }

    Shaders::instance().activate(ShaderProgram::GrassGrass, uniforms);
    Meshes::instance().getQuadFlipY()->drawInstanced(static_cast<int>(_clusters.size()));
}

} // namespace scene

} // namespace reone
