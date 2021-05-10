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

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

using namespace std;

namespace reone {

namespace graphics {

Mesh::Mesh(int vertexCount, vector<float> vertices, vector<uint16_t> indices, VertexOffsets offsets, DrawMode mode) :
    _vertexCount(vertexCount),
    _vertices(move(vertices)),
    _indices(move(indices)),
    _offsets(move(offsets)),
    _mode(mode) {
}

void Mesh::init() {
    if (_inited) return;

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), &_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint16_t), &_indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.vertexCoords));

    if (_offsets.normals != -1) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.normals));
    }
    if (_offsets.texCoords1 != -1) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.texCoords1));
    }
    if (_offsets.texCoords2 != -1) {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.texCoords2));
    }
    if (_offsets.tangents != -1) {
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.tangents));
    }
    if (_offsets.bitangents != -1) {
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.bitangents));
    }
    if (_offsets.boneWeights != -1) {
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.boneWeights));
    }
    if (_offsets.boneIndices != -1) {
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.boneIndices));
    }

    glBindVertexArray(0);

    _inited = true;
}

Mesh::~Mesh() {
    deinit();
}

void Mesh::deinit() {
    if (_inited) {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_ibo);
        glDeleteBuffers(1, &_vbo);
        _inited = false;
    }
}

static GLenum getModeGL(Mesh::DrawMode mode) {
    switch (mode) {
        case Mesh::DrawMode::Lines:
            return GL_LINES;
        case Mesh::DrawMode::TriangleStrip:
            return GL_TRIANGLE_STRIP;
        default:
            return GL_TRIANGLES;
    }
}

void Mesh::draw() {
    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glDrawElements(getModeGL(_mode), static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void Mesh::drawInstanced(int count) {
    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glDrawElementsInstanced(getModeGL(_mode), static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_SHORT, nullptr, count);
}

void Mesh::computeAABB() {
    _aabb.reset();

    int stride = _offsets.stride;
    if (stride == 0) {
        stride = 3 * sizeof(float);
    }
    auto vertCoords = reinterpret_cast<uint8_t *>(&_vertices[0]) + _offsets.vertexCoords;

    for (size_t i = 0; i < _vertexCount; ++i) {
        _aabb.expand(glm::make_vec3(reinterpret_cast<const float *>(vertCoords)));
        vertCoords += stride;
    }
}

glm::vec2 Mesh::getFaceCenterUV(int faceIdx) const {
    if (faceIdx < 0 || faceIdx >= _indices.size() / 3) {
        throw out_of_range("faceIdx out of range");
    }
    if (_offsets.texCoords1 == -1) return glm::vec2(0.0f);

    glm::vec2 result(0.0f);
    const uint16_t *indices = &_indices[3 * faceIdx];
    for (int i = 0; i < 3; ++i) {
        const float *uv = &_vertices[(indices[i] * _offsets.stride + _offsets.texCoords1) / sizeof(float)];
        result.x += uv[0];
        result.y += uv[1];
    }
    result /= 3.0f;
    result = glm::clamp(result);

    return move(result);
}

} // namespace graphics

} // namespace reone
