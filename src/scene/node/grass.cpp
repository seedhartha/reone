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

#include "../../graphics/context.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"

#include "../graph.h"

#include "grasscluster.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

void GrassSceneNode::drawElements(const vector<SceneNode *> &elements, int count) {
    if (elements.empty()) {
        return;
    }
    if (count == -1) {
        count = static_cast<int>(elements.size());
    }

    _context.setActiveTextureUnit(TextureUnits::diffuseMap);
    _texture->bind();

    ShaderUniforms uniforms(_sceneGraph.uniformsPrototype());
    uniforms.combined.featureMask |= UniformFeatureFlags::grass;

    if (_lightmap) {
        _context.setActiveTextureUnit(TextureUnits::lightmap);
        _lightmap->bind();

        uniforms.combined.featureMask |= UniformFeatureFlags::lightmap;
    }

    for (int i = 0; i < count; ++i) {
        auto cluster = static_cast<GrassClusterSceneNode *>(elements[i]);
        uniforms.grass->quadSize = _quadSize;
        uniforms.grass->clusters[i].positionVariant = glm::vec4(cluster->position(), static_cast<float>(cluster->variant()));
        uniforms.grass->clusters[i].lightmapUV = cluster->lightmapUV();
    }

    _shaders.activate(ShaderProgram::GrassGrass, uniforms);
    _meshes.grass().drawInstanced(count);
}

unique_ptr<GrassClusterSceneNode> GrassSceneNode::newCluster() {
    return make_unique<GrassClusterSceneNode>(_sceneGraph, _context, _meshes, _shaders);
}

} // namespace scene

} // namespace reone
