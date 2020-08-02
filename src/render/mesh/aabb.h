#pragma once

#include "mesh.h"

namespace reone {

namespace render {

class AABBMesh : public Mesh {
public:
    static AABBMesh &instance();
    void render(const AABB &aabb, const glm::mat4 &transform) const;

private:
    AABBMesh();
};

} // namespace render

} // namespace reone
