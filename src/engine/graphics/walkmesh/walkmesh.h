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

#pragma once

#include "../aabb.h"
#include "../types.h"

namespace reone {

namespace graphics {

class BwmReader;

class Walkmesh : boost::noncopyable {
public:
    struct Face {
        int index {0};
        uint32_t material {0};
        std::vector<glm::vec3> vertices;
        glm::vec3 normal {0.0f};

        bool walkable {false}; // can be determined from material
    };

    struct AABB {
        graphics::AABB value;
        int faceIdx {-1};
        std::shared_ptr<AABB> child1;
        std::shared_ptr<AABB> child2;
    };

    /**
     * @return pointer to intersected face or nullptr, when no intersection
     */
    const Walkmesh::Face *raycast(const glm::vec3 &origin, const glm::vec3 &dir, float maxDistance, float &outDistance) const;

private:
    std::vector<Face> _faces;
    std::shared_ptr<AABB> _rootAabb;

    const Walkmesh::Face *raycastAABB(AABB &aabb, const glm::vec3 &origin, const glm::vec3 &dir, float maxDistance, float &outDistance) const;

    bool raycastFace(const Walkmesh::Face &face, const glm::vec3 &origin, const glm::vec3 &dir, float maxDistance, float &outDistance) const;

    friend class BwmReader;
};

} // namespace graphics

} // namespace reone
