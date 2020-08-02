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
