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
    _vertexCount = _vertices.size() / attributes.stride / sizeof(float);
}

void Mesh::init() {
    if (_inited) return;

    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ibo);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), &_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
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
    if (_attributes.offBoneWeights != -1) {
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offBoneWeights));
    }
    if (_attributes.offBoneIndices != -1) {
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, _attributes.stride, reinterpret_cast<void *>(_attributes.offBoneIndices));
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
    glDrawElements(getModeGL(_mode), static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void Mesh::drawInstanced(int count) {
    glBindVertexArray(_vao);
    glDrawElementsInstanced(getModeGL(_mode), static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_SHORT, nullptr, count);
}

void Mesh::computeAABB() {
    _aabb.reset();

    auto coordsPtr = reinterpret_cast<uint8_t *>(&_vertices[0]) + _attributes.offCoords;

    for (size_t i = 0; i < _vertexCount; ++i) {
        _aabb.expand(glm::make_vec3(reinterpret_cast<const float *>(coordsPtr)));
        coordsPtr += _attributes.stride;
    }
}

glm::vec2 Mesh::getFaceCenterUV(int faceIdx) const {
    if (_mode != DrawMode::Triangles) {
        throw logic_error("Unsupported draw mode: " + to_string(static_cast<int>(_mode)));
    }
    if (faceIdx < 0 || faceIdx >= static_cast<int>(_indices.size()) / 3) {
        throw out_of_range("faceIdx out of range");
    }
    if (_attributes.offTexCoords1 == -1) {
        return glm::vec2(0.0f);
    }

    glm::vec2 result(0.0f);
    const uint16_t *indices = &_indices[3 * faceIdx];
    for (int i = 0; i < 3; ++i) {
        const float *uv = &_vertices[(indices[i] * _attributes.stride + _attributes.offTexCoords1) / sizeof(float)];
        result.x += uv[0];
        result.y += uv[1];
    }
    result /= 3.0f;
    result = glm::clamp(result);

    return move(result);
}

} // namespace graphics

} // namespace reone
