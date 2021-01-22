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

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

namespace reone {

namespace render {

void Mesh::computeAABB() {
    _aabb.reset();

    const uint8_t *vertCoords = reinterpret_cast<uint8_t *>(&_vertices[0]) + _offsets.vertexCoords;

    int stride = _offsets.stride;
    if (stride == 0) {
        stride = 3 * sizeof(float);
    }

    for (size_t i = 0; i < _vertexCount; ++i) {
        _aabb.expand(glm::make_vec3(reinterpret_cast<const float *>(vertCoords)));
        vertCoords += stride;
    }
}

void Mesh::initGL() {
    if (_glInited) return;

    glGenBuffers(1, &_vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), &_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &_indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint16_t), &_indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_vertexArrayId);
    glBindVertexArray(_vertexArrayId);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);

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

    _glInited = true;
}

Mesh::~Mesh() {
    deinitGL();
}

void Mesh::deinitGL() {
    if (!_glInited) return;

    glDeleteVertexArrays(1, &_vertexArrayId);
    glDeleteBuffers(1, &_indexBufferId);
    glDeleteBuffers(1, &_vertexBufferId);

    _glInited = false;
}

void Mesh::renderLines() const {
    render(GL_LINES, static_cast<int>(_indices.size()), 0);
}

void Mesh::renderTriangles() const {
    render(GL_TRIANGLES, static_cast<int>(_indices.size()), 0);
}

void Mesh::render(uint32_t mode, int count, int offset) const {
    glBindVertexArray(_vertexArrayId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);

    glDrawElements(mode, count, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(offset));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

const AABB &Mesh::aabb() const {
    return _aabb;
}

} // namespace render

} // namespace reone
