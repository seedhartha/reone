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

#include "reone/scene/node/walkmesh.h"

#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/material.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/statistic.h"
#include "reone/graphics/uniforms.h"
#include "reone/scene/graph.h"
#include "reone/scene/render/pipeline.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

void WalkmeshSceneNode::init() {
    std::vector<float> vertices;
    std::vector<Mesh::Face> faces;

    for (auto &wface : _walkmesh.faces()) {
        size_t vertIdxStart = vertices.size() / 7;
        for (int i = 0; i < 3; ++i) {
            vertices.push_back(wface.vertices[i].x);
            vertices.push_back(wface.vertices[i].y);
            vertices.push_back(wface.vertices[i].z);
            vertices.push_back(wface.normal.x);
            vertices.push_back(wface.normal.y);
            vertices.push_back(wface.normal.z);
            float material = glm::min(1.0f, static_cast<int>(wface.material) / static_cast<float>(kMaxWalkmeshMaterials - 1));
            vertices.push_back(material);
        }
        Mesh::Face face;
        face.vertices[0] = vertIdxStart + 0;
        face.vertices[1] = vertIdxStart + 1;
        face.vertices[2] = vertIdxStart + 2;
        face.material = wface.material;
        faces.push_back(std::move(face));
    }

    Mesh::VertexLayout vertexLayout;
    vertexLayout.stride = 7 * sizeof(float);
    vertexLayout.offPosition = 0;
    vertexLayout.offNormals = 3 * sizeof(float);
    vertexLayout.offMaterial = 6 * sizeof(float);

    _mesh = std::make_unique<Mesh>(
        std::move(vertices),
        std::move(vertexLayout),
        std::move(faces));
    _mesh->init();
}

void WalkmeshSceneNode::render(IRenderPass &pass) {
    Material material;
    material.type = MaterialType::Walkmesh;
    material.faceCulling = FaceCullMode::Back;
    pass.draw(*_mesh, material, _absTransform, _absTransformInv);
}

} // namespace scene

} // namespace reone
