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

#include "mesh.h"

#include <stdexcept>
#include <string>

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

#include "../baryutil.h"

using namespace std;

namespace reone {

namespace graphics {

Mesh::Mesh(vector<float> vertices, vector<uint16_t> indices, VertexAttributes attributes, DrawMode mode) :
    _vertices(move(vertices)),
    _indices(move(indices)),
    _attributes(move(attributes)),
    _mode(mode) {

    if (attributes.stride == 0) {
        throw invalid_argument("stride in attributes must not be zero");
    }
    _vertexCount = _vertices.size() / (attributes.stride / sizeof(float));

    computeAABB();
}

void Mesh::computeAABB() {
    _aabb.reset();

    auto coordsPtr = reinterpret_cast<uint8_t *>(&_vertices[0]) + _attributes.offCoords;

    for (size_t i = 0; i < _vertexCount; ++i) {
        _aabb.expand(glm::make_vec3(reinterpret_cast<const float *>(coordsPtr)));
        coordsPtr += _attributes.stride;
    }
}

void Mesh::init() {
    if (_inited) return;

    glGenBuffers(1, &_vboId);
    glGenBuffers(1, &_iboId);

    glGenVertexArrays(1, &_vaoId);
    glBindVertexArray(_vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, _vboId);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), &_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint16_t), &_indices[0], GL_STATIC_DRAW);

    if (_attributes.offCoords != -1) {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offCoords));
    }
    if (_attributes.offNormals != -1) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offNormals));
    }
    if (_attributes.offTexCoords1 != -1) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offTexCoords1));
    }
    if (_attributes.offTexCoords2 != -1) {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offTexCoords2));
    }
    if (_attributes.offTangents != -1) {
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offTangents));
    }
    if (_attributes.offBitangents != -1) {
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offBitangents));
    }
    if (_attributes.offTanSpaceNormals != -1) {
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offTanSpaceNormals));
    }
    if (_attributes.offBoneIndices != -1) {
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offBoneIndices));
    }
    if (_attributes.offBoneWeights != -1) {
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offBoneWeights));
    }

    glBindVertexArray(0);

    _inited = true;
}

Mesh::~Mesh() {
    deinit();
}

void Mesh::deinit() {
    if (!_inited) return;

    glDeleteVertexArrays(1, &_vaoId);
    glDeleteBuffers(1, &_iboId);
    glDeleteBuffers(1, &_vboId);

    _inited = false;
}

static inline GLenum getModeGL(Mesh::DrawMode mode) {
    switch (mode) {
        case Mesh::DrawMode::Lines:
            return GL_LINES;
        case Mesh::DrawMode::Triangles:
            return GL_TRIANGLES;
        case Mesh::DrawMode::TriangleStrip:
            return GL_TRIANGLE_STRIP;
        default:
            throw invalid_argument("Unsupported draw mode: " + to_string(static_cast<int>(mode)));
    }
}

void Mesh::draw() {
    glBindVertexArray(_vaoId);
    glDrawElements(getModeGL(_mode), static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void Mesh::drawInstanced(int count) {
    glBindVertexArray(_vaoId);
    glDrawElementsInstanced(getModeGL(_mode), static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_SHORT, nullptr, count);
}

void Mesh::ensureTriangles() const {
    if (_mode != DrawMode::Triangles) {
        throw logic_error("Unsupported draw mode: " + to_string(static_cast<int>(_mode)));
    }
}

void Mesh::drawTriangles(int startFace, int numFaces) {
    ensureTriangles();
    glBindVertexArray(_vaoId);
    glDrawElements(GL_TRIANGLES, 3 * numFaces, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(3 * startFace * sizeof(uint16_t)));
}

void Mesh::drawTrianglesInstanced(int startFace, int numFaces, int count) {
    ensureTriangles();
    glBindVertexArray(_vaoId);
    glDrawElementsInstanced(GL_TRIANGLES, 3 * numFaces, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(3 * startFace * sizeof(uint16_t)), count);
}

vector<glm::vec3> Mesh::getTriangleCoords(int faceIdx) const {
    return getTriangleAttributes<glm::vec3>(faceIdx, _attributes.offCoords);
}

glm::vec2 Mesh::getTriangleTexCoords1(int faceIdx, const glm::vec3 &baryPosition) const {
    auto points = getTriangleAttributes<glm::vec2>(faceIdx, _attributes.offTexCoords1);

    return !points.empty() ?
        barycentricToCartesian(points[0], points[1], points[2], baryPosition) :
        glm::vec2(0.0f);
}

glm::vec2 Mesh::getTriangleTexCoords2(int faceIdx, const glm::vec3 &baryPosition) const {
    auto points = getTriangleAttributes<glm::vec2>(faceIdx, _attributes.offTexCoords2);

    return !points.empty() ?
        barycentricToCartesian(points[0], points[1], points[2], baryPosition) :
        glm::vec2(0.0f);
}

template <class T>
vector<T> Mesh::getTriangleAttributes(int faceIdx, int offset) const {
    ensureTriangles();
    if (faceIdx < 0 || faceIdx >= static_cast<int>(_indices.size()) / 3) {
        throw out_of_range("faceIdx out of range");
    }
    if (offset == -1) return vector<T>();

    auto a = getVertexAttribute<T>(_indices[3 * faceIdx + 0], offset);
    auto b = getVertexAttribute<T>(_indices[3 * faceIdx + 1], offset);
    auto c = getVertexAttribute<T>(_indices[3 * faceIdx + 2], offset);

    return vector<T> { a, b, c };
}

template <>
glm::vec2 Mesh::getVertexAttribute(uint16_t vertexIdx, int offset) const {
    return glm::make_vec2(&_vertices[(vertexIdx * _attributes.stride + offset) / sizeof(float)]);
}

template <>
glm::vec3 Mesh::getVertexAttribute(uint16_t vertexIdx, int offset) const {
    return glm::make_vec3(&_vertices[(vertexIdx * _attributes.stride + offset) / sizeof(float)]);
}

} // namespace graphics

} // namespace reone
