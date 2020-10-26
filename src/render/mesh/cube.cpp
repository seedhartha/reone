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

#include "cube.h"

#include "SDL2/SDL_opengl.h"

#include "../shaders.h"

using namespace std;

namespace reone {

namespace render {

static vector<float> g_vertices = {
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f
};

static vector<uint16_t> g_indices = {
    0, 1, 2, 2, 3, 0,
    2, 4, 5, 5, 3, 2,
    1, 7, 4, 4, 2, 1,
    0, 6, 7, 7, 1, 0,
    7, 6, 5, 5, 4, 7,
    6, 0, 3, 3, 5, 6
};

static Mesh::VertexOffsets g_offsets = { 0, -1, -1, -1, -1, -1, 3 * sizeof(float) };

CubeMesh &CubeMesh::instance() {
    static CubeMesh mesh;
    return mesh;
}

CubeMesh::CubeMesh() {
    _vertices = move(g_vertices);
    _indices = move(g_indices);
    _offsets = move(g_offsets);
}

void CubeMesh::render(const glm::mat4 &transform) const {
    LocalUniforms locals;
    locals.model = transform;

    Shaders::instance().activate(ShaderProgram::ModelWhite, locals);

    Mesh::render(GL_TRIANGLES);
}

} // namespace render

} // namespace reone
