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

#include <stdexcept>

#include "../../render/meshes.h"
#include "../../render/shaders.h"
#include "../../render/stateutil.h"

#include "../scenegraph.h"

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

void GrassSceneNode::drawClusters(const vector<GrassCluster> &clusters) {
    setActiveTextureUnit(TextureUnits::diffuse);
    _texture->bind();

    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    uniforms.general.featureMask |= UniformFeatureFlags::grass;

    int numClusters = static_cast<int>(clusters.size());
    for (int i = 0; i < numClusters; ++i) {
        uniforms.grass.quadSize = _quadSize;
        uniforms.grass.clusters[i].positionVariant = glm::vec4(clusters[i].position, static_cast<float>(clusters[i].variant));
    }

    Shaders::instance().activate(ShaderProgram::GrassGrass, uniforms);
    Meshes::instance().getQuadFlipY()->drawInstanced(numClusters);
}

} // namespace scene

} // namespace reone
