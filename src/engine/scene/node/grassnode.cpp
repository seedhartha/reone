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

#include "../../graphics/mesh/meshes.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/statemanager.h"

#include "../scenegraph.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

GrassSceneNode::GrassSceneNode(SceneGraph *graph, glm::vec2 quadSize, shared_ptr<Texture> texture, shared_ptr<Texture> lightmap) :
    SceneNode(SceneNodeType::Grass, graph),
    _quadSize(move(quadSize)),
    _texture(texture),
    _lightmap(move(lightmap)) {

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
    StateManager::instance().setActiveTextureUnit(TextureUnits::diffuse);
    _texture->bind();

    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    uniforms.combined.featureMask |= UniformFeatureFlags::grass;

    if (_lightmap) {
        StateManager::instance().setActiveTextureUnit(TextureUnits::lightmap);
        _lightmap->bind();

        uniforms.combined.featureMask |= UniformFeatureFlags::lightmap;
    }

    int numClusters = static_cast<int>(clusters.size());
    for (int i = 0; i < numClusters; ++i) {
        uniforms.grass->quadSize = _quadSize;
        uniforms.grass->clusters[i].positionVariant = glm::vec4(clusters[i].position, static_cast<float>(clusters[i].variant));
        uniforms.grass->clusters[i].lightmapUV = clusters[i].lightmapUV;
    }

    Shaders::instance().activate(ShaderProgram::GrassGrass, uniforms);
    Meshes::instance().getGrass()->drawInstanced(numClusters);
}

} // namespace scene

} // namespace reone
