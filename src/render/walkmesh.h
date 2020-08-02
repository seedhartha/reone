#pragma once

#include <vector>

#include "aabb.h"

namespace reone {

namespace resources {

class BwmFile;

}

namespace render {

class Walkmesh {
public:
    struct Face {
        uint32_t type { 0 };
        std::vector<uint16_t> indices;
    };

    Walkmesh() = default;

    bool findObstacle(const glm::vec3 &from, const glm::vec3 &to, glm::vec3 &intersection) const;
    bool findElevationAt(const glm::vec3 &position, float &z) const;

    const AABB &aabb() const;

private:
    std::vector<glm::vec3> _vertices;
    std::vector<Face> _walkableFaces;
    std::vector<Face> _nonWalkableFaces;
    AABB _aabb;

    Walkmesh(const Walkmesh &) = delete;
    Walkmesh &operator=(const Walkmesh &) = delete;

    void computeAABB();

    friend class resources::BwmFile;
};

} // namespace render

} // namespace reone
