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

#pragma once

#include <vector>

#include "aabb.h"

namespace reone {

namespace render {

class BwmFile;

class Walkmesh {
public:
    Walkmesh() = default;

    bool raycast(const glm::vec3 &origin, const glm::vec3 &dir, bool walkable, float &distance) const;

    const AABB &aabb() const;

private:
    struct Face {
        uint32_t type { 0 };
        std::vector<uint16_t> indices;
    };

    std::vector<glm::vec3> _vertices;
    std::vector<Face> _walkableFaces;
    std::vector<Face> _nonWalkableFaces;
    AABB _aabb;

    Walkmesh(const Walkmesh &) = delete;
    Walkmesh &operator=(const Walkmesh &) = delete;

    void computeAABB();

    friend class BwmFile;
};

} // namespace render

} // namespace reone
