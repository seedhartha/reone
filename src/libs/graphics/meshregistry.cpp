
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

// Quads

static const std::vector<float> g_quadVertices {
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f};

static const std::vector<float> g_quadNDCVertices {
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f};

static const std::vector<float> g_billboardVertices {
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};

static const std::vector<float> g_grassVertices {
    -0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 1.0f, 0.0f, 0.0f, 1.0f};

static const std::vector<Mesh::Face> g_quadFaces {
    Mesh::Face(0, 1, 2),
    Mesh::Face(2, 3, 0)};

static const Mesh::VertexSpec g_quadSpec {5 * sizeof(float), 0, -1, 3 * sizeof(float)};

// END Quads

// AABB

static const std::vector<float> g_aabbVertices {
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f //
};

static const std::vector<Mesh::Face> g_aabbFaces {
    // left quad
    Mesh::Face(0, 1, 2),
    Mesh::Face(1, 3, 2),

    // right quad
    Mesh::Face(4, 6, 7),
    Mesh::Face(4, 7, 5),

    // back quad
    Mesh::Face(0, 2, 4),
    Mesh::Face(2, 6, 4),

    // front quad
    Mesh::Face(1, 7, 3),
    Mesh::Face(1, 5, 7),

    // bottom face
    Mesh::Face(0, 5, 1),
    Mesh::Face(0, 4, 5),

    // top face
    Mesh::Face(2, 3, 7),
    Mesh::Face(2, 7, 6) //
};

static const Mesh::VertexSpec g_aabbSpec {3 * sizeof(float), 0};

// END AABB

// Cubemap

static const std::vector<float> g_cubemapVertices {
    // back face
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
    -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
    // front face
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
    // left face
    -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    // right face
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
    1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
    // bottom face
    -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
    1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    // top face
    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
    -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
};

static const std::vector<Mesh::Face> g_cubemapFaces {
    Mesh::Face(0, 1, 2),
    Mesh::Face(1, 0, 3),
    Mesh::Face(4, 5, 6),
    Mesh::Face(6, 7, 4),
    Mesh::Face(8, 9, 10),
    Mesh::Face(10, 11, 8),
    Mesh::Face(12, 13, 14),
    Mesh::Face(13, 12, 15),
    Mesh::Face(16, 17, 18),
    Mesh::Face(18, 19, 16),
    Mesh::Face(20, 21, 22),
    Mesh::Face(21, 20, 23)};

static const Mesh::VertexSpec g_cubemapSpec {8 * sizeof(float), 0, 3 * sizeof(float), 6 * sizeof(float)};

// END Cubemap

static std::unique_ptr<Mesh> makeMesh(std::vector<float> vertices,
                                      std::vector<Mesh::Face> faces,
                                      Mesh::VertexSpec spec,
                                      Statistic &statistic) {
    return std::make_unique<Mesh>(
        std::move(vertices),
        std::move(faces),
        std::move(spec),
        statistic);
}

void MeshRegistry::init() {
    if (_inited) {
        return;
    }
    add(MeshName::quad, makeMesh(g_quadVertices, g_quadFaces, g_quadSpec, _statistic));
    add(MeshName::quadNDC, makeMesh(g_quadNDCVertices, g_quadFaces, g_quadSpec, _statistic));
    add(MeshName::billboard, makeMesh(g_billboardVertices, g_quadFaces, g_quadSpec, _statistic));
    add(MeshName::grass, makeMesh(g_grassVertices, g_quadFaces, g_quadSpec, _statistic));
    add(MeshName::aabb, makeMesh(g_aabbVertices, g_aabbFaces, g_aabbSpec, _statistic));
    add(MeshName::cubemap, makeMesh(g_cubemapVertices, g_cubemapFaces, g_cubemapSpec, _statistic));
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
