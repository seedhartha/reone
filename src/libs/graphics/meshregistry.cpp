
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

#include "reone/graphics/meshregistry.h"

#include "reone/graphics/mesh.h"
#include "reone/graphics/statistic.h"

namespace reone {

namespace graphics {

static const Mesh::VertexLayout kPositionVertexLayout =
    Mesh::VertexLayoutBuilder()
        .stride(3 * sizeof(float))
        .offPosition(0)
        .build();

static const Mesh::VertexLayout kPositionNormalUV1VertexLayout =
    Mesh::VertexLayoutBuilder()
        .stride(8 * sizeof(float))
        .offPosition(0)
        .offNormals(3 * sizeof(float))
        .offUV1(6 * sizeof(float))
        .build();

static const Mesh::VertexLayout kPositionUV1VertexLayout =
    Mesh::VertexLayoutBuilder()
        .stride(5 * sizeof(float))
        .offPosition(0)
        .offUV1(3 * sizeof(float))
        .build();

// Quads

static const std::vector<Mesh::Face> kQuadFaces {
    Mesh::Face {{0, 1, 2}},
    Mesh::Face {{2, 3, 0}}};

static const std::vector<Mesh::Vertex> kQuadVertices {
    Mesh::VertexBuilder().position({0.0f, 0.0f, 0.0f}).uv1({0.0f, 1.0f}).build(),
    Mesh::VertexBuilder().position({1.0f, 0.0f, 0.0f}).uv1({1.0f, 1.0f}).build(),
    Mesh::VertexBuilder().position({1.0f, 1.0f, 0.0f}).uv1({1.0f, 0.0f}).build(),
    Mesh::VertexBuilder().position({0.0f, 1.0f, 0.0f}).uv1({0.0f, 0.0f}).build()};

static const std::vector<Mesh::Vertex> kNDCQuadVertices {
    Mesh::VertexBuilder().position({-1.0f, 1.0f, 0.0f}).uv1({0.0f, 1.0f}).build(),
    Mesh::VertexBuilder().position({1.0f, 1.0f, 0.0f}).uv1({1.0f, 1.0f}).build(),
    Mesh::VertexBuilder().position({1.0f, -1.0f, 0.0f}).uv1({1.0f, 0.0f}).build(),
    Mesh::VertexBuilder().position({-1.0f, -1.0f, 0.0f}).uv1({0.0f, 0.0f}).build()};

static const std::vector<Mesh::Vertex> kBillboardVertices {
    Mesh::VertexBuilder().position({-0.5f, -0.5f, 0.0f}).uv1({0.0f, 0.0f}).build(),
    Mesh::VertexBuilder().position({0.5f, -0.5f, 0.0f}).uv1({1.0f, 0.0f}).build(),
    Mesh::VertexBuilder().position({0.5f, 0.5f, 0.0f}).uv1({1.0f, 1.0f}).build(),
    Mesh::VertexBuilder().position({-0.5f, 0.5f, 0.0f}).uv1({0.0f, 1.0f}).build()};

static const std::vector<Mesh::Vertex> kGrassVertices {
    Mesh::VertexBuilder().position({-0.5f, 0.0f, 0.0f}).uv1({0.0f, 0.0f}).build(),
    Mesh::VertexBuilder().position({0.5f, 0.0f, 0.0f}).uv1({1.0f, 0.0f}).build(),
    Mesh::VertexBuilder().position({0.5f, 1.0f, 0.0f}).uv1({1.0f, 1.0f}).build(),
    Mesh::VertexBuilder().position({-0.5f, 1.0f, 0.0f}).uv1({0.0f, 1.0f}).build()};

// END Quads

// AABB

static const std::vector<Mesh::Face> kAABBFaces {
    // left quad
    Mesh::Face {{0, 1, 2}},
    Mesh::Face {{1, 3, 2}},

    // right quad
    Mesh::Face {{4, 6, 7}},
    Mesh::Face {{4, 7, 5}},

    // back quad
    Mesh::Face {{0, 2, 4}},
    Mesh::Face {{2, 6, 4}},

    // front quad
    Mesh::Face {{1, 7, 3}},
    Mesh::Face {{1, 5, 7}},

    // bottom face
    Mesh::Face {{0, 5, 1}},
    Mesh::Face {{0, 4, 5}},

    // top face
    Mesh::Face {{2, 3, 7}},
    Mesh::Face {{2, 7, 6}}};

static const std::vector<Mesh::Vertex> kAABBVertices {
    Mesh::VertexBuilder().position({-1.0f, -1.0f, -1.0f}).build(),
    Mesh::VertexBuilder().position({-1.0f, -1.0f, 1.0f}).build(),
    Mesh::VertexBuilder().position({-1.0f, 1.0f, -1.0f}).build(),
    Mesh::VertexBuilder().position({-1.0f, 1.0f, 1.0f}).build(),
    Mesh::VertexBuilder().position({1.0f, -1.0f, -1.0f}).build(),
    Mesh::VertexBuilder().position({1.0f, -1.0f, 1.0f}).build(),
    Mesh::VertexBuilder().position({1.0f, 1.0f, -1.0f}).build(),
    Mesh::VertexBuilder().position({1.0f, 1.0f, 1.0f}).build()};

// END AABB

// Cubemap

static const std::vector<Mesh::Face> kCubemapFaces {
    Mesh::Face {{0, 1, 2}},
    Mesh::Face {{1, 0, 3}},
    Mesh::Face {{4, 5, 6}},
    Mesh::Face {{6, 7, 4}},
    Mesh::Face {{8, 9, 10}},
    Mesh::Face {{10, 11, 8}},
    Mesh::Face {{12, 13, 14}},
    Mesh::Face {{13, 12, 15}},
    Mesh::Face {{16, 17, 18}},
    Mesh::Face {{18, 19, 16}},
    Mesh::Face {{20, 21, 22}},
    Mesh::Face {{21, 20, 23}}};

static const std::vector<Mesh::Vertex> kCubemapVertices {
    // back face
    Mesh::VertexBuilder().position({-1.0f, -1.0f, -1.0f}).normal({0.0f, 0.0f, -1.0f}).uv1({0.0f, 0.0f}).build(), // bottom-left
    Mesh::VertexBuilder().position({1.0f, 1.0f, -1.0f}).normal({0.0f, 0.0f, -1.0f}).uv1({1.0f, 1.0f}).build(),   // top-right
    Mesh::VertexBuilder().position({1.0f, -1.0f, -1.0f}).normal({0.0f, 0.0f, -1.0f}).uv1({1.0f, 0.0f}).build(),  // bottom-right
    Mesh::VertexBuilder().position({-1.0f, 1.0f, -1.0f}).normal({0.0f, 0.0f, -1.0f}).uv1({0.0f, 1.0f}).build(),  // top-left
    // front face
    Mesh::VertexBuilder().position({-1.0f, -1.0f, 1.0f}).normal({0.0f, 0.0f, 1.0f}).uv1({0.0f, 0.0f}).build(), // bottom-left
    Mesh::VertexBuilder().position({1.0f, -1.0f, 1.0f}).normal({0.0f, 0.0f, 1.0f}).uv1({1.0f, 0.0f}).build(),  // bottom-right
    Mesh::VertexBuilder().position({1.0f, 1.0f, 1.0f}).normal({0.0f, 0.0f, 1.0f}).uv1({1.0f, 1.0f}).build(),   // top-right
    Mesh::VertexBuilder().position({-1.0f, 1.0f, 1.0f}).normal({0.0f, 0.0f, 1.0f}).uv1({0.0f, 1.0f}).build(),  // top-left
    // left face
    Mesh::VertexBuilder().position({-1.0f, 1.0f, 1.0f}).normal({-1.0f, 0.0f, 0.0f}).uv1({1.0f, 0.0f}).build(),   // top-right
    Mesh::VertexBuilder().position({-1.0f, 1.0f, -1.0f}).normal({-1.0f, 0.0f, 0.0f}).uv1({1.0f, 1.0f}).build(),  // top-left
    Mesh::VertexBuilder().position({-1.0f, -1.0f, -1.0f}).normal({-1.0f, 0.0f, 0.0f}).uv1({0.0f, 1.0f}).build(), // bottom-left
    Mesh::VertexBuilder().position({-1.0f, -1.0f, 1.0f}).normal({-1.0f, 0.0f, 0.0f}).uv1({0.0f, 0.0f}).build(),  // bottom-right
    // right face
    Mesh::VertexBuilder().position({1.0f, 1.0f, 1.0f}).normal({1.0f, 0.0f, 0.0f}).uv1({1.0f, 0.0f}).build(),   // top-left
    Mesh::VertexBuilder().position({1.0f, -1.0f, -1.0f}).normal({1.0f, 0.0f, 0.0f}).uv1({0.0f, 1.0f}).build(), // bottom-right
    Mesh::VertexBuilder().position({1.0f, 1.0f, -1.0f}).normal({1.0f, 0.0f, 0.0f}).uv1({1.0f, 1.0f}).build(),  // top-right
    Mesh::VertexBuilder().position({1.0f, -1.0f, 1.0f}).normal({1.0f, 0.0f, 0.0f}).uv1({0.0f, 0.0f}).build(),  // bottom-left
    // bottom face
    Mesh::VertexBuilder().position({-1.0f, -1.0f, -1.0f}).normal({0.0f, -1.0f, 0.0f}).uv1({0.0f, 1.0f}).build(), // top-right
    Mesh::VertexBuilder().position({1.0f, -1.0f, -1.0f}).normal({0.0f, -1.0f, 0.0f}).uv1({1.0f, 1.0f}).build(),  // top-left
    Mesh::VertexBuilder().position({1.0f, -1.0f, 1.0f}).normal({0.0f, -1.0f, 0.0f}).uv1({1.0f, 0.0f}).build(),   // bottom-left
    Mesh::VertexBuilder().position({-1.0f, -1.0f, 1.0f}).normal({0.0f, -1.0f, 0.0f}).uv1({0.0f, 0.0f}).build(),  // bottom-right
    // top face
    Mesh::VertexBuilder().position({-1.0f, 1.0f, -1.0f}).normal({0.0f, 1.0f, 0.0f}).uv1({0.0f, 1.0f}).build(), // top-left
    Mesh::VertexBuilder().position({1.0f, 1.0f, 1.0f}).normal({0.0f, 1.0f, 0.0f}).uv1({1.0f, 0.0f}).build(),   // bottom-right
    Mesh::VertexBuilder().position({1.0f, 1.0f, -1.0f}).normal({0.0f, 1.0f, 0.0f}).uv1({1.0f, 1.0f}).build(),  // top-right
    Mesh::VertexBuilder().position({-1.0f, 1.0f, 1.0f}).normal({0.0f, 1.0f, 0.0f}).uv1({0.0f, 0.0f}).build()   // bottom-left
};

// END Cubemap

static std::unique_ptr<Mesh> makeMesh(std::vector<Mesh::Vertex> vertices,
                                      std::vector<Mesh::Face> faces,
                                      Mesh::VertexLayout vertexLayout,
                                      Statistic &statistic) {
    auto mesh = std::make_unique<Mesh>(
        std::move(vertices),
        std::move(vertexLayout),
        std::move(faces));
    mesh->init();
    return mesh;
}

void MeshRegistry::init() {
    if (_inited) {
        return;
    }
    add(MeshName::quad, makeMesh(kQuadVertices, kQuadFaces, kPositionUV1VertexLayout, _statistic));
    add(MeshName::quadNDC, makeMesh(kNDCQuadVertices, kQuadFaces, kPositionUV1VertexLayout, _statistic));
    add(MeshName::billboard, makeMesh(kBillboardVertices, kQuadFaces, kPositionUV1VertexLayout, _statistic));
    add(MeshName::grass, makeMesh(kGrassVertices, kQuadFaces, kPositionUV1VertexLayout, _statistic));
    add(MeshName::aabb, makeMesh(kAABBVertices, kAABBFaces, kPositionVertexLayout, _statistic));
    add(MeshName::cubemap, makeMesh(kCubemapVertices, kCubemapFaces, kPositionNormalUV1VertexLayout, _statistic));
    _inited = true;
}

void MeshRegistry::deinit() {
    if (!_inited) {
        return;
    }
    _nameToMesh.clear();
    _inited = false;
}

} // namespace graphics

} // namespace reone
