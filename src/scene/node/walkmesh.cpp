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

#include "walkmesh.h"

#include "../../graphics/context.h"
#include "../../graphics/shaders.h"

#include "../graph.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

void WalkmeshSceneNode::init() {
    vector<float> vertices;
    vector<Mesh::Face> faces;

    for (auto &wface : _walkmesh->faces()) {
        size_t vertIdxStart = vertices.size() / 7;
        for (int i = 0; i < 3; ++i) {
            vertices.push_back(wface.vertices[i].x);
            vertices.push_back(wface.vertices[i].y);
            vertices.push_back(wface.vertices[i].z);
            vertices.push_back(wface.normal.x);
            vertices.push_back(wface.normal.y);
            vertices.push_back(wface.normal.z);
            vertices.push_back(glm::min(kMaxWalkmeshMaterials, static_cast<int>(wface.material)) / static_cast<float>(kMaxWalkmeshMaterials));
        }
        Mesh::Face face;
        face.indices[0] = vertIdxStart + 0;
        face.indices[1] = vertIdxStart + 1;
        face.indices[2] = vertIdxStart + 2;
        face.material = wface.material;
        faces.push_back(move(face));
    }

    Mesh::VertexSpec spec;
    spec.stride = 7 * sizeof(float);
    spec.offCoords = 0;
    spec.offNormals = 3 * sizeof(float);
    spec.offMaterial = 6 * sizeof(float);

    _mesh = make_unique<Mesh>(move(vertices), move(faces), move(spec));
    _mesh->init();
}

void WalkmeshSceneNode::draw() {
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetLocals();
    uniforms.general.model = _absTransform;

    _shaders.use(_shaders.walkmesh(), true);
    _graphicsContext.withFaceCulling(CullFaceMode::Back, [this]() {
        _mesh->draw();
    });
}

} // namespace scene

} // namespace reone
