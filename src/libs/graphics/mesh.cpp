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

#include "reone/graphics/mesh.h"

#include "reone/graphics/barycentricutil.h"
#include "reone/graphics/statistic.h"
#include "reone/graphics/triangleutil.h"
#include "reone/system/threadutil.h"

namespace reone {

namespace graphics {

void Mesh::init() {
    if (_inited) {
        return;
    }
    checkMainThread();

    std::vector<uint16_t> indices;
    indices.reserve(3 * _faces.size());
    for (auto &face : _faces) {
        indices.push_back(face.indices[0]);
        indices.push_back(face.indices[1]);
        indices.push_back(face.indices[2]);
    }

    // OpenGL

    glGenBuffers(1, &_vboId);
    glGenBuffers(1, &_iboId);

    glGenVertexArrays(1, &_vaoId);
    glBindVertexArray(_vaoId);
    if (!_vertices.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, _vboId);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), &_vertices[0], GL_STATIC_DRAW);
    }
    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), &indices[0], GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(0));
    if (_spec.offNormals != -1) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(static_cast<size_t>(_spec.offNormals)));
    }
    if (_spec.offUV1 != -1) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(static_cast<size_t>(_spec.offUV1)));
    }
    if (_spec.offUV2 != -1) {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(static_cast<size_t>(_spec.offUV2)));
    }
    if (_spec.offTanSpace != -1) {
        // Bitangents
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(static_cast<size_t>(_spec.offTanSpace)));
        // Tangents
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(static_cast<size_t>(_spec.offTanSpace + 3 * sizeof(float))));
        // Normals
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(static_cast<size_t>(_spec.offTanSpace + 6 * sizeof(float))));
    }
    if (_spec.offBoneIndices != -1) {
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(static_cast<size_t>(_spec.offBoneIndices)));
    }
    if (_spec.offBoneWeights != -1) {
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(static_cast<size_t>(_spec.offBoneWeights)));
    }
    if (_spec.offMaterial != -1) {
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, _spec.stride, reinterpret_cast<void *>(static_cast<size_t>(_spec.offMaterial)));
    }

    glBindVertexArray(0);

    // END OpenGL

    _inited = true;
}

void Mesh::deinit() {
    if (!_inited) {
        return;
    }
    checkMainThread();
    glDeleteVertexArrays(1, &_vaoId);
    glDeleteBuffers(1, &_iboId);
    glDeleteBuffers(1, &_vboId);
    _inited = false;
}

void Mesh::draw() {
    if (!_inited) {
        init();
    }
    glBindVertexArray(_vaoId);
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(3 * _faces.size()),
        GL_UNSIGNED_SHORT,
        nullptr);
    _statistic.incrementDrawCalls();
}

void Mesh::drawInstanced(int count) {
    if (!_inited) {
        init();
    }
    glBindVertexArray(_vaoId);
    glDrawElementsInstanced(
        GL_TRIANGLES,
        static_cast<GLsizei>(3 * _faces.size()),
        GL_UNSIGNED_SHORT,
        nullptr,
        count);
    _statistic.incrementDrawCalls();
}

void Mesh::computeFaceData() {
    for (auto &face : _faces) {
        std::vector<glm::vec3> verts(getFaceVertexCoords(face));
        // Normal
        if (face.normal[0] == 0.0f && face.normal[1] == 0.0f && face.normal[2] == 0.0f) {
            face.normal = calculateTriangleNormal(verts);
        }
        // Centroid
        if (face.centroid[0] == 0.0f || face.centroid[1] == 0.0f || face.centroid[2] == 0.0f) {
            face.centroid = (verts[0] + verts[1] + verts[2]) / 3.0f;
        }
        // Area
        if (face.area == 0.0f) {
            face.area = calculateTriangleArea(verts);
        }
    }
}

void Mesh::computeAABB() {
    _aabb.reset();

    auto valsPerVert = _spec.stride / sizeof(float);
    auto numVerts = _vertices.size() / valsPerVert;
    for (auto i = 0; i < numVerts; ++i) {
        auto vertPtr = &_vertices[i * valsPerVert];
        _aabb.expand(glm::make_vec3(&vertPtr[_spec.offCoords / sizeof(float)]));
    }
}

std::vector<glm::vec3> Mesh::getVertexCoords() const {
    std::vector<glm::vec3> coords;
    coords.resize(_numVertices);
    for (int i = 0; i < _numVertices; ++i) {
        auto vertPtr = &_vertices[i * (_spec.stride / sizeof(float))];
        coords[i] = glm::make_vec3(&vertPtr[_spec.offCoords / sizeof(float)]);
    }
    return coords;
}

std::vector<glm::vec3> Mesh::getFaceVertexCoords(const Face &face) const {
    std::vector<glm::vec3> coords(3);
    for (int i = 0; i < 3; ++i) {
        auto vertPtr = &_vertices[face.indices[i] * (_spec.stride / sizeof(float))];
        coords[i] = glm::make_vec3(&vertPtr[_spec.offCoords / sizeof(float)]);
    }
    return coords;
}

glm::vec2 Mesh::getUV1(const Face &face, const glm::vec3 &baryPosition) const {
    std::vector<glm::vec2> uv(3);
    for (int i = 0; i < 3; ++i) {
        auto vertPtr = &_vertices[face.indices[i] * (_spec.stride / sizeof(float))];
        uv[i] = glm::make_vec2(&vertPtr[_spec.offUV1 / sizeof(float)]);
    }
    return barycentricToCartesian(uv[0], uv[1], uv[2], baryPosition);
}

glm::vec2 Mesh::getUV2(const Face &face, const glm::vec3 &baryPosition) const {
    std::vector<glm::vec2> uv(3);
    for (int i = 0; i < 3; ++i) {
        auto vertPtr = &_vertices[face.indices[i] * (_spec.stride / sizeof(float))];
        uv[i] = glm::make_vec2(&vertPtr[_spec.offUV2 / sizeof(float)]);
    }
    return barycentricToCartesian(uv[0], uv[1], uv[2], baryPosition);
}

} // namespace graphics

} // namespace reone
