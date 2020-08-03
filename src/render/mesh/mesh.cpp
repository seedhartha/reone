/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <cassert>

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

namespace reone {

namespace render {

void Mesh::computeAABB() {
    _aabb.reset();

    const float *pv = &_vertices[_offsets.vertexCoords / sizeof(float)];
    int stride = _offsets.stride / sizeof(float);
    int vertexCount = _vertices.size() / stride;

    for (int i = 0; i < vertexCount; ++i) {
        _aabb.expand(glm::make_vec3(pv));
        pv += stride;
    }
}

void Mesh::initGL() {
    if (_glInited) return;

    assert(!_vertices.empty() && !_indices.empty());

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
    if (_offsets.boneWeights != -1) {
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.boneWeights));
    }
    if (_offsets.boneIndices != -1) {
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, _offsets.stride, reinterpret_cast<void *>(_offsets.boneIndices));
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

void Mesh::render(uint32_t mode) const {
    render(mode, _indices.size(), 0);
}

void Mesh::render(uint32_t mode, int count, int offset) const {
    assert(_glInited);

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
