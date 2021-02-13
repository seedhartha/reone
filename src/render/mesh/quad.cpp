
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

#include "quad.h"

using namespace std;

namespace reone {

namespace render {

static vector<float> g_defaultVertices {
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f
};

static vector<float> g_flipXVertices {
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 1.0f, 0.0f
};

static vector<float> g_flipYVertices {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

static vector<float> g_flipXYVertices {
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f
};

static vector<float> g_ndcVertices {
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f
};

static vector<uint16_t> g_indices = { 0, 1, 2, 2, 3, 0 };

static Mesh::VertexOffsets g_offsets = { 0, -1, 3 * sizeof(float), -1, -1, -1, -1, -1, 5 * sizeof(float) };

Quad &Quad::getDefault() {
    static Quad quad(move(g_defaultVertices));
    return quad;
}

Quad &Quad::getXFlipped() {
    static Quad quad(move(g_flipXVertices));
    return quad;
}

Quad &Quad::getYFlipped() {
    static Quad quad(move(g_flipYVertices));
    return quad;
}

Quad &Quad::getXYFlipped() {
    static Quad quad(move(g_flipXYVertices));
    return quad;
}

Quad &Quad::getNDC() {
    static Quad quad(move(g_ndcVertices));
    return quad;
}

Quad::Quad(vector<float> &&vertices) : Mesh(4, move(vertices), g_indices, g_offsets) {
}

} // namespace render

} // namespace reone
