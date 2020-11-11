/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "walkmesh.h"

#include "glm/gtx/intersect.hpp"
#include "glm/gtx/norm.hpp"

using namespace std;

namespace reone {

namespace render {

void Walkmesh::computeAABB() {
    _aabb.reset();

    for (auto &vert : _vertices) {
        _aabb.expand(vert);
    }
}

bool Walkmesh::raycast(const glm::vec3 &origin, const glm::vec3 &dir, bool walkable, float maxDistance, float &distance) const {
    const vector<Face> &faces = walkable ? _walkableFaces : _nonWalkableFaces;
    glm::vec2 baryPosition(0.0f);

    for (auto &face : faces) {
        const glm::vec3 &p0 = _vertices[face.indices[0]];
        const glm::vec3 &p1 = _vertices[face.indices[1]];
        const glm::vec3 &p2 = _vertices[face.indices[2]];

        if (glm::intersectRayTriangle(origin, dir, p0, p1, p2, baryPosition, distance) && distance >= 0.0f && distance <= maxDistance) {
            return true;
        }
    }

    return false;
}

const AABB &Walkmesh::aabb() const {
    return _aabb;
}

} // namespace render

} // namespace reone
