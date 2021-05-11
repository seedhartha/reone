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

#include <boost/noncopyable.hpp>

#include "../aabb.h"

#include "vertexattributes.h"

namespace reone {

namespace graphics {

/**
 * Polygonal mesh, containing vertex and index data. Renders itself,
 * but does not manage textures and shaders.
 */
class Mesh : boost::noncopyable {
public:
    enum class DrawMode {
        Lines,
        Triangles,
        TriangleStrip
    };

    Mesh(std::vector<float> vertices, std::vector<uint16_t> indices, VertexAttributes attributes, DrawMode mode = DrawMode::Triangles);
    ~Mesh();

    void init();
    void deinit();

    void draw();
    void drawInstanced(int count);

    void computeAABB();

    /**
     * Used for lightmapping grass.
     *
     * @return UV coordinates at face center
     */
    glm::vec2 getFaceCenterUV(int faceIdx) const;

    const std::vector<float> &vertices() const { return _vertices; }
    const std::vector<uint16_t> &indices() const { return _indices; }
    const VertexAttributes &attributes() const { return _attributes; }
    VertexAttributes &attributes() { return _attributes; }
    const AABB &aabb() const { return _aabb; }

private:
    std::vector<float> _vertices;
    std::vector<uint16_t> _indices;
    VertexAttributes _attributes;
    DrawMode _mode;

    int _vertexCount { 0 };
    bool _inited { false };
    AABB _aabb;

    // OpenGL

    uint32_t _vbo { 0 };
    uint32_t _ibo { 0 };
    uint32_t _vao { 0 };

    // END OpenGL
};

} // namespace graphics

} // namespace reone
