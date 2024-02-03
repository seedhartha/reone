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
#include "reone/system/checkutil.h"
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
        indices.push_back(face.vertices[0]);
        indices.push_back(face.vertices[1]);
        indices.push_back(face.vertices[2]);
    }

    // OpenGL

    glGenBuffers(1, &_vboId);
    glGenBuffers(1, &_iboId);

    glGenVertexArrays(1, &_vaoId);
    glBindVertexArray(_vaoId);
    if (!_vertexData.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, _vboId);
        glBufferData(GL_ARRAY_BUFFER, _vertexData.size() * sizeof(float), &_vertexData[0], GL_STATIC_DRAW);
    }
    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), &indices[0], GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(0));
    if (_vertexLayout.offNormals != -1) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(static_cast<size_t>(_vertexLayout.offNormals)));
    }
    if (_vertexLayout.offUV1 != -1) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(static_cast<size_t>(_vertexLayout.offUV1)));
    }
    if (_vertexLayout.offUV2 != -1) {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(static_cast<size_t>(_vertexLayout.offUV2)));
    }
    if (_vertexLayout.offTanSpace != -1) {
        // Bitangents
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(static_cast<size_t>(_vertexLayout.offTanSpace)));
        // Tangents
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(static_cast<size_t>(_vertexLayout.offTanSpace + 3 * sizeof(float))));
        // Normals
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(static_cast<size_t>(_vertexLayout.offTanSpace + 6 * sizeof(float))));
    }
    if (_vertexLayout.offBoneIndices != -1) {
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(static_cast<size_t>(_vertexLayout.offBoneIndices)));
    }
    if (_vertexLayout.offBoneWeights != -1) {
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(static_cast<size_t>(_vertexLayout.offBoneWeights)));
    }
    if (_vertexLayout.offMaterial != -1) {
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, _vertexLayout.stride, reinterpret_cast<void *>(static_cast<size_t>(_vertexLayout.offMaterial)));
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

void Mesh::draw(IStatistic &statistic) {
    glBindVertexArray(_vaoId);
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(3 * _faces.size()),
        GL_UNSIGNED_SHORT,
        nullptr);
    statistic.incrementDrawCalls();
}

void Mesh::drawInstanced(int count, IStatistic &statistic) {
    glBindVertexArray(_vaoId);
    glDrawElementsInstanced(
        GL_TRIANGLES,
        static_cast<GLsizei>(3 * _faces.size()),
        GL_UNSIGNED_SHORT,
        nullptr,
        count);
    statistic.incrementDrawCalls();
}

void Mesh::computeVertexDataFromVertices() {
    _vertexData.resize(_vertices.size() * _vertexLayout.stride / sizeof(float));
    for (size_t i = 0; i < _vertices.size(); ++i) {
        const auto &vertex = _vertices[i];
        float *dataPtr;
        dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offPosition) / sizeof(float)];
        std::memcpy(dataPtr, glm::value_ptr(vertex.position), 3 * sizeof(float));
        if (_vertexLayout.offNormals != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offNormals) / sizeof(float)];
            std::memcpy(dataPtr, glm::value_ptr(*vertex.normal), 3 * sizeof(float));
        }
        if (_vertexLayout.offUV1 != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offUV1) / sizeof(float)];
            std::memcpy(dataPtr, glm::value_ptr(*vertex.uv1), 2 * sizeof(float));
        }
        if (_vertexLayout.offUV2 != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offUV2) / sizeof(float)];
            std::memcpy(dataPtr, glm::value_ptr(*vertex.uv2), 2 * sizeof(float));
        }
        if (_vertexLayout.offTanSpace != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offTanSpace) / sizeof(float)];
            std::memcpy(dataPtr, glm::value_ptr(*vertex.bitangent), 3 * sizeof(float));
            dataPtr = &_vertexData[3 + (i * _vertexLayout.stride + _vertexLayout.offTanSpace) / sizeof(float)];
            std::memcpy(dataPtr, glm::value_ptr(*vertex.tangent), 3 * sizeof(float));
            dataPtr = &_vertexData[6 + (i * _vertexLayout.stride + _vertexLayout.offTanSpace) / sizeof(float)];
            std::memcpy(dataPtr, glm::value_ptr(*vertex.tanSpaceNormal), 3 * sizeof(float));
        }
        if (_vertexLayout.offBoneIndices != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offBoneIndices) / sizeof(float)];
            std::memcpy(dataPtr, glm::value_ptr(*vertex.boneIndices), 4 * sizeof(float));
        }
        if (_vertexLayout.offBoneWeights != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offBoneWeights) / sizeof(float)];
            std::memcpy(dataPtr, glm::value_ptr(*vertex.boneWeights), 4 * sizeof(float));
        }
        if (_vertexLayout.offMaterial != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offMaterial) / sizeof(float)];
            *dataPtr = static_cast<float>(*vertex.material);
        }
    }
}

void Mesh::computeVerticesFromVertexData() {
    size_t numVertices = _vertexData.size() / (_vertexLayout.stride / sizeof(float));
    _vertices.resize(numVertices);
    for (size_t i = 0; i < numVertices; ++i) {
        Vertex vertex;
        float *dataPtr;
        dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offPosition) / sizeof(float)];
        vertex.position = glm::make_vec3(dataPtr);
        if (_vertexLayout.offNormals != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offNormals) / sizeof(float)];
            vertex.normal = glm::make_vec3(dataPtr);
        }
        if (_vertexLayout.offUV1 != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offUV1) / sizeof(float)];
            vertex.uv1 = glm::make_vec2(dataPtr);
        }
        if (_vertexLayout.offUV2 != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offUV2) / sizeof(float)];
            vertex.uv2 = glm::make_vec2(dataPtr);
        }
        if (_vertexLayout.offTanSpace != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offTanSpace) / sizeof(float)];
            vertex.bitangent = glm::make_vec3(dataPtr);
            dataPtr = &_vertexData[3 + (i * _vertexLayout.stride + _vertexLayout.offTanSpace) / sizeof(float)];
            vertex.tangent = glm::make_vec3(dataPtr);
            dataPtr = &_vertexData[6 + (i * _vertexLayout.stride + _vertexLayout.offTanSpace) / sizeof(float)];
            vertex.tanSpaceNormal = glm::make_vec3(dataPtr);
        }
        if (_vertexLayout.offBoneIndices != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offBoneIndices) / sizeof(float)];
            vertex.boneIndices = glm::make_vec4(dataPtr);
        }
        if (_vertexLayout.offBoneWeights != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offBoneWeights) / sizeof(float)];
            vertex.boneWeights = glm::make_vec4(dataPtr);
        }
        if (_vertexLayout.offMaterial != -1) {
            dataPtr = &_vertexData[(i * _vertexLayout.stride + _vertexLayout.offMaterial) / sizeof(float)];
            vertex.material = static_cast<int>(*dataPtr);
        }
        _vertices.push_back(std::move(vertex));
    }
}

void Mesh::computeFaceData() {
    for (auto &face : _faces) {
        std::vector<glm::vec3> verts(faceVertexCoords(face));
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
    for (const auto &vertex : _vertices) {
        _aabb.expand(vertex.position);
    }
}

std::vector<glm::vec3> Mesh::vertexCoords() const {
    std::vector<glm::vec3> coords;
    coords.reserve(_vertices.size());
    for (const auto &vertex : _vertices) {
        coords.push_back(vertex.position);
    }
    return coords;
}

std::vector<glm::vec3> Mesh::faceVertexCoords(const Face &face) const {
    std::vector<glm::vec3> coords;
    coords.reserve(3);
    for (int i = 0; i < 3; ++i) {
        const auto &vertex = _vertices[face.vertices[i]];
        coords.push_back(vertex.position);
    }
    return coords;
}

glm::vec2 Mesh::faceUV1(const Face &face, const glm::vec3 &baryPosition) const {
    checkNotEqual("UV offset", _vertexLayout.offUV1, 1);
    std::vector<glm::vec2> uv;
    uv.reserve(3);
    for (int i = 0; i < 3; ++i) {
        const auto &vertex = _vertices[face.vertices[i]];
        uv.push_back(*vertex.uv1);
    }
    return barycentricToCartesian(uv[0], uv[1], uv[2], baryPosition);
}

glm::vec2 Mesh::faceUV2(const Face &face, const glm::vec3 &baryPosition) const {
    checkNotEqual("UV offset", _vertexLayout.offUV2, 1);
    std::vector<glm::vec2> uv;
    uv.reserve(3);
    for (int i = 0; i < 3; ++i) {
        const auto &vertex = _vertices[face.vertices[i]];
        uv.push_back(*vertex.uv2);
    }
    return barycentricToCartesian(uv[0], uv[1], uv[2], baryPosition);
}

} // namespace graphics

} // namespace reone
