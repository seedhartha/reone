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

#include "guiquad.h"

namespace reone {

namespace render {

static std::vector<float> g_vertices = {
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f
};

static std::vector<uint16_t> g_indices = { 0, 1, 2, 2, 3, 0 };

static Mesh::VertexOffsets g_offsets = { 0, -1, 3 * sizeof(float), -1, -1, -1, 5 * sizeof(float) };

GUIQuad &GUIQuad::instance() {
    static GUIQuad quad;
    return quad;
}

GUIQuad::GUIQuad() {
    _vertices = g_vertices;
    _indices = g_indices;
    _offsets = g_offsets;
}

} // namespace render

} // namespace reone
