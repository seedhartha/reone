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
 * Polygon mesh. Consists of vertices and faces.
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

    void drawTriangles(int startFace, int numFaces);
    void drawTrianglesInstanced(int startFace, int numFaces, int count);

    /**
     * @param faceIdx faceIdx
     * @return coordinates of three triangle vertices
     */
    std::vector<glm::vec3> getTriangleCoords(int faceIdx) const;

    /**
    * @param faceIdx face index
    * @param baryPosition barycentric point coordinates
    * @return first texture coordinates
    */
    glm::vec2 getTriangleTexCoords1(int faceIdx, const glm::vec3 &baryPosition) const;

    /**
     * @param faceIdx face index
     * @param baryPosition barycentric point coordinates
     * @return second texture coordinates
     */
    glm::vec2 getTriangleTexCoords2(int faceIdx, const glm::vec3 &baryPosition) const;

    const std::vector<float> &vertices() const { return _vertices; }
    const std::vector<uint16_t> &indices() const { return _indices; }
    const VertexAttributes &attributes() const { return _attributes; }
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

    uint32_t _vboId { 0 };
    uint32_t _iboId { 0 };
    uint32_t _vaoId { 0 };

    // END OpenGL

    void computeAABB();

    glm::vec3 getVertexCoords(uint16_t vertexIdx) const;
    glm::vec2 getVertexTexCoords1(uint16_t vertexIdx) const;
    glm::vec2 getVertexTexCoords2(uint16_t vertexIdx) const;

    inline void ensureTriangles() const;
};

} // namespace graphics

} // namespace reone
