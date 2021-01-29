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

#pragma once

#include <cstdint>
#include <vector>

#include "../../common/aabb.h"

namespace reone {

namespace render {

class MdlFile;

/**
 * Polygonal mesh, containing vertex and index data. Renders itself,
 * but does not manage textures and shaders.
 */
class Mesh {
public:
    struct VertexOffsets {
        int vertexCoords { 0 };
        int normals { -1 };
        int texCoords1 { -1 };
        int texCoords2 { -1 };
        int tangents { -1 };
        int bitangents { -1 };
        int boneWeights { -1 };
        int boneIndices { -1 };
        int stride { 0 };
    };

    void initGL();
    void deinitGL();
    void renderLines() const;
    void renderTriangles() const;

    const AABB &aabb() const { return _aabb; }

protected:
    bool _glInited { false };
    int _vertexCount { 0 };
    std::vector<float> _vertices;
    std::vector<uint16_t> _indices;
    VertexOffsets _offsets;
    uint32_t _indexBufferId { 0 };
    uint32_t _vertexArrayId { 0 };

    Mesh() = default;
    virtual ~Mesh();

    void computeAABB();
    void render(uint32_t mode, int count, int offset) const;

private:
    uint32_t _vertexBufferId { 0 };
    AABB _aabb;

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    friend class MdlFile;
};

} // namespace render

} // namespace reone
