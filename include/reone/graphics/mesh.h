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

#pragma once

#include "aabb.h"

namespace reone {

namespace graphics {

class IStatistic;

class Mesh : boost::noncopyable {
public:
    struct VertexSpec {
        int stride {0};
        int offCoords {-1};
        int offNormals {-1};
        int offUV1 {-1};
        int offUV2 {-1};
        int offTanSpace {-1};
        int offBoneIndices {-1};
        int offBoneWeights {-1};
        int offMaterial {-1};
    };

    struct Face {
        uint16_t indices[3] {0};
        uint16_t adjacentFaces[3] {0xffff};
        uint32_t material {0};
        glm::vec3 normal {0.0f};
        glm::vec3 centroid {0.0f};
        float area {0.0f};

        Face() = default;

        Face(uint16_t i1, uint16_t i2, uint16_t i3) {
            indices[0] = i1;
            indices[1] = i2;
            indices[2] = i3;
        }
    };

    Mesh(std::vector<float> vertices,
         std::vector<Face> faces,
         VertexSpec spec,
         IStatistic &statistic) :
        _numVertices(vertices.size() / (spec.stride / sizeof(float))),
        _vertices(std::move(vertices)),
        _faces(std::move(faces)),
        _spec(std::move(spec)),
        _statistic(statistic) {
        computeFaceData();
        computeAABB();
    }

    ~Mesh() { deinit(); }

    void init();
    void deinit();

    void draw();
    void drawInstanced(int count);

    std::vector<glm::vec3> getVertexCoords() const;
    std::vector<glm::vec3> getFaceVertexCoords(const Face &face) const;
    glm::vec2 getUV1(const Face &face, const glm::vec3 &baryPosition) const;
    glm::vec2 getUV2(const Face &face, const glm::vec3 &baryPosition) const;

    int numVertices() const { return _numVertices; }
    const std::vector<Face> &faces() const { return _faces; }
    const AABB &aabb() const { return _aabb; }

private:
    int _numVertices;
    std::vector<float> _vertices;
    std::vector<Face> _faces;
    VertexSpec _spec;
    IStatistic &_statistic;

    AABB _aabb;
    bool _inited {false};

    // OpenGL

    uint32_t _vboId {0};
    uint32_t _iboId {0};
    uint32_t _vaoId {0};

    // END OpenGL

    void computeFaceData();
    void computeAABB();
};

} // namespace graphics

} // namespace reone
