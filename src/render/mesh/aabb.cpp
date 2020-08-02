#include "aabb.h"

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../shadermanager.h"

namespace reone {

namespace render {

static std::vector<float> g_vertices = {
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f
};

static std::vector<uint16_t> g_indices = {
    0, 1, 1, 2, 2, 3, 3, 0,
    2, 4, 4, 5, 5, 3, 3, 2,
    1, 7, 7, 4, 4, 2, 2, 1,
    0, 6, 6, 7, 7, 1, 1, 0,
    7, 6, 6, 5, 5, 4, 4, 7,
    6, 0, 0, 3, 3, 5, 5, 6
};

static Mesh::VertexOffsets g_offsets = { 0, -1, -1, -1, -1, -1, 3 * sizeof(float) };

AABBMesh &AABBMesh::instance() {
    static AABBMesh mesh;
    return mesh;
}

AABBMesh::AABBMesh() {
    _vertices = g_vertices;
    _indices = g_indices;
    _offsets = g_offsets;
}

void AABBMesh::render(const AABB &aabb, const glm::mat4 &transform) const {
    glm::mat4 transform2(transform * aabb.transform());

    ShaderManager &shaders = ShaderManager::instance();
    shaders.activate(ShaderProgram::BasicWhite);
    shaders.setUniform("model", transform2);
    shaders.setUniform("alpha", 1.0f);

    Mesh::render(GL_LINES);

    shaders.deactivate();
}

} // namespace render

} // namespace reone
