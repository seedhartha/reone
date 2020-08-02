#pragma once

#include <cstdint>
#include <vector>

#include "../aabb.h"

namespace reone {

namespace resources {

class MdlFile;

}

namespace render {

/**
 * Polygonal mesh, containing vertex and index data. Renders itself,
 * but does not manage textures and shaders.
 */
class Mesh {
public:
    struct VertexOffsets {
        int vertexCoords { 0 };
        int normals { -1 };
        int texCoords1 { -1 };
        int texCoords2 { -1 };
        int boneWeights { -1 };
        int boneIndices { -1 };
        int stride { 0 };
    };

    void initGL();
    void deinitGL();
    void render(uint32_t mode) const;

    const AABB &aabb() const;

protected:
    bool _glInited { false };
    std::vector<float> _vertices;
    std::vector<uint16_t> _indices;
    VertexOffsets _offsets;
    uint32_t _indexBufferId { 0 };
    uint32_t _vertexArrayId { 0 };

    Mesh() = default;
    virtual ~Mesh();

    void computeAABB();
    void render(uint32_t mode, int count, int offset) const;

private:
    uint32_t _vertexBufferId { 0 };
    AABB _aabb;

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    friend class resources::MdlFile;
};

} // namespace render

} // namespace reone
