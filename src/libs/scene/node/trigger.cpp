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

#include "reone/scene/node/trigger.h"

#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/shaders.h"
#include "reone/graphics/uniforms.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

void TriggerSceneNode::init() {
    size_t numVertices = _geometry.size();

    std::vector<glm::vec3> normals(numVertices + 1, glm::vec3(0.0f));
    std::vector<Mesh::Face> faces;

    glm::vec3 centroid(0.0f);
    for (auto &vert : _geometry) {
        centroid += vert;
    }
    centroid /= static_cast<float>(numVertices);

    for (size_t i = 0; i < numVertices; ++i) {
        int i1 = i + 1;
        int i2 = (i == numVertices - 1) ? 1 : (i + 2);

        auto normal = glm::cross(_geometry[i1 - 1] - centroid, _geometry[i2 - 1] - centroid);
        normals[0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;

        Mesh::Face face;
        face.indices[0] = 0;
        face.indices[1] = i1;
        face.indices[2] = i2;
        face.normal = std::move(normal);
        face.material = kMaxWalkmeshMaterials - 1;
        faces.push_back(std::move(face));
    }

    for (size_t i = 0; i < normals.size(); ++i) {
        normals[i] = glm::normalize(normals[i]);
    }

    std::vector<float> vertices;
    vertices.push_back(centroid.x);
    vertices.push_back(centroid.y);
    vertices.push_back(centroid.z);
    vertices.push_back(normals[0].x);
    vertices.push_back(normals[0].y);
    vertices.push_back(normals[0].z);
    vertices.push_back(1.0f); // material
    for (size_t i = 0; i < numVertices; ++i) {
        vertices.push_back(_geometry[i].x);
        vertices.push_back(_geometry[i].y);
        vertices.push_back(_geometry[i].z);
        vertices.push_back(normals[i + 1].x);
        vertices.push_back(normals[i + 1].y);
        vertices.push_back(normals[i + 1].z);
        vertices.push_back(1.0f); // material
    }

    Mesh::VertexSpec spec;
    spec.stride = 7 * sizeof(float);
    spec.offCoords = 0;
    spec.offNormals = 3 * sizeof(float);
    spec.offMaterial = 6 * sizeof(float);

    _mesh = std::make_unique<Mesh>(std::move(vertices), std::move(faces), std::move(spec));

    for (auto &point : _geometry) {
        _aabb.expand(point);
    }
}

void TriggerSceneNode::draw() {
    _graphicsSvc.uniforms.setGeneral([this](auto &general) {
        general.resetLocals();
        general.model = _absTransform;
    });
    _graphicsSvc.shaders.use(ShaderProgramId::Walkmesh);
    _graphicsSvc.context.withFaceCulling(CullFaceMode::Back, [this]() {
        _mesh->draw();
    });
}

bool TriggerSceneNode::isIn(const glm::vec2 &pt) const {
    static glm::vec3 down(0.0f, 0.0f, -1.0f);

    auto pointObjSpace = glm::vec3(_absTransformInv * glm::vec4(pt, 1000.0f, 1.0f));
    auto intersection = glm::vec2(0.0f);
    float distance = 0.0f;

    for (auto &face : _mesh->faces()) {
        auto verts = _mesh->getVertexCoords(face);
        if (glm::intersectRayTriangle(pointObjSpace, down, verts[0], verts[1], verts[2], intersection, distance)) {
            return true;
        }
    }

    return false;
}

} // namespace scene

} // namespace reone
