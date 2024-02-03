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
    struct Vertex {
        glm::vec3 position {0.0f};
        std::optional<glm::vec3> normal;
        std::optional<glm::vec2> uv1;
        std::optional<glm::vec2> uv2;
        std::optional<glm::vec3> tangent;
        std::optional<glm::vec3> bitangent;
        std::optional<glm::vec3> tanSpaceNormal;
        std::optional<glm::ivec4> boneIndices;
        std::optional<glm::vec4> boneWeights;
        std::optional<int> material;
    };

    class VertexBuilder {
    public:
        VertexBuilder &position(glm::vec3 position) {
            _position = std::move(position);
            return *this;
        }

        VertexBuilder &normal(glm::vec3 normal) {
            _normal = std::move(normal);
            return *this;
        }

        VertexBuilder &uv1(glm::vec2 uv1) {
            _uv1 = std::move(uv1);
            return *this;
        }

        VertexBuilder &uv2(glm::vec2 uv2) {
            _uv2 = std::move(uv2);
            return *this;
        }

        VertexBuilder &tangent(glm::vec3 tangent) {
            _tangent = std::move(tangent);
            return *this;
        }

        VertexBuilder &bitangent(glm::vec3 bitangent) {
            _bitangent = std::move(bitangent);
            return *this;
        }

        VertexBuilder &tanSpaceNormal(glm::vec3 normal) {
            _tanSpaceNormal = std::move(normal);
            return *this;
        }

        VertexBuilder &boneIndices(glm::ivec4 boneIndices) {
            _boneIndices = std::move(boneIndices);
            return *this;
        }

        VertexBuilder &boneWeights(glm::vec4 boneWeights) {
            _boneWeights = std::move(boneWeights);
            return *this;
        }

        VertexBuilder &material(int material) {
            _material = material;
            return *this;
        }

        Vertex build() {
            Vertex vertex;
            vertex.position = _position;
            vertex.normal = _normal;
            vertex.uv1 = _uv1;
            vertex.uv2 = _uv2;
            vertex.tangent = _tangent;
            vertex.bitangent = _bitangent;
            vertex.tanSpaceNormal = _tanSpaceNormal;
            vertex.boneIndices = _boneIndices;
            vertex.boneWeights = _boneWeights;
            vertex.material = _material;
            return vertex;
        }

    private:
        glm::vec3 _position {0.0f};
        std::optional<glm::vec3> _normal;
        std::optional<glm::vec2> _uv1;
        std::optional<glm::vec2> _uv2;
        std::optional<glm::vec3> _tangent;
        std::optional<glm::vec3> _bitangent;
        std::optional<glm::vec3> _tanSpaceNormal;
        std::optional<glm::ivec4> _boneIndices;
        std::optional<glm::vec4> _boneWeights;
        std::optional<int> _material;
    };

    struct VertexLayout {
        int stride {0};
        int offPosition {-1};
        int offNormals {-1};
        int offUV1 {-1};
        int offUV2 {-1};
        int offTanSpace {-1};
        int offBoneIndices {-1};
        int offBoneWeights {-1};
        int offMaterial {-1};
    };

    class VertexLayoutBuilder {
    public:
        VertexLayoutBuilder &stride(int stride) {
            _stride = stride;
            return *this;
        }

        VertexLayoutBuilder &offPosition(int offset) {
            _offPosition = offset;
            return *this;
        }

        VertexLayoutBuilder &offNormals(int offset) {
            _offNormals = offset;
            return *this;
        }

        VertexLayoutBuilder &offUV1(int offset) {
            _offUV1 = offset;
            return *this;
        }

        VertexLayoutBuilder &offUV2(int offset) {
            _offUV2 = offset;
            return *this;
        }

        VertexLayoutBuilder &offTanSpace(int offset) {
            _offTanSpace = offset;
            return *this;
        }

        VertexLayoutBuilder &offBoneIndices(int offset) {
            _offBoneIndices = offset;
            return *this;
        }

        VertexLayoutBuilder &offBoneWeights(int offset) {
            _offBoneWeights = offset;
            return *this;
        }

        VertexLayoutBuilder &offMaterial(int offset) {
            _offMaterial = offset;
            return *this;
        }

        VertexLayout build() {
            VertexLayout layout;
            layout.stride = _stride;
            layout.offPosition = _offPosition;
            layout.offNormals = _offNormals;
            layout.offUV1 = _offUV1;
            layout.offUV2 = _offUV2;
            layout.offTanSpace = _offTanSpace;
            layout.offBoneIndices = _offBoneIndices;
            layout.offBoneWeights = _offBoneWeights;
            layout.offMaterial = _offMaterial;
            return layout;
        }

    private:
        int _stride {0};
        int _offPosition {-1};
        int _offNormals {-1};
        int _offUV1 {-1};
        int _offUV2 {-1};
        int _offTanSpace {-1};
        int _offBoneIndices {-1};
        int _offBoneWeights {-1};
        int _offMaterial {-1};
    };

    struct Face {
        std::array<uint16_t, 3> vertices {0};
        std::array<uint16_t, 3> adjacentFaces {0xffff};
        uint32_t material {0};
        glm::vec3 normal {0.0f};
        glm::vec3 centroid {0.0f};
        float area {0.0f};

        Face() = default;

        Face(std::array<uint16_t, 3> vertices) :
            vertices(std::move(vertices)) {
        }
    };

    Mesh(std::vector<Vertex> vertices,
         VertexLayout vertexLayout,
         std::vector<Face> faces) :
        _vertices(std::move(vertices)),
        _vertexLayout(std::move(vertexLayout)),
        _faces(std::move(faces)) {
        computeVertexDataFromVertices();
        computeFaceData();
        computeAABB();
    }

    Mesh(std::vector<float> vertexData,
         VertexLayout vertexLayout,
         std::vector<Face> faces) :
        _vertexData(std::move(vertexData)),
        _vertexLayout(std::move(vertexLayout)),
        _faces(std::move(faces)) {
        computeVerticesFromVertexData();
        computeFaceData();
        computeAABB();
    }

    ~Mesh() { deinit(); }

    void init();
    void deinit();

    void draw(IStatistic &statistic);
    void drawInstanced(int count, IStatistic &statistic);

    std::vector<glm::vec3> vertexCoords() const;
    std::vector<glm::vec3> faceVertexCoords(const Face &face) const;
    glm::vec2 faceUV1(const Face &face, const glm::vec3 &baryPosition) const;
    glm::vec2 faceUV2(const Face &face, const glm::vec3 &baryPosition) const;

    int vertexCount() const { return _vertices.size(); }
    const std::vector<Face> &faces() const { return _faces; }
    const AABB &aabb() const { return _aabb; }

private:
    VertexLayout _vertexLayout;
    std::vector<Face> _faces;

    bool _inited {false};

    std::vector<Vertex> _vertices;
    std::vector<float> _vertexData;
    AABB _aabb;

    // OpenGL

    uint32_t _vboId {0};
    uint32_t _iboId {0};
    uint32_t _vaoId {0};

    // END OpenGL

    void computeVertexDataFromVertices();
    void computeVerticesFromVertexData();
    void computeFaceData();
    void computeAABB();
};

} // namespace graphics

} // namespace reone
