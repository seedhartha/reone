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

#include "walkmesh.h"

#include "glm/gtx/intersect.hpp"
#include "glm/gtx/norm.hpp"

namespace reone {

namespace render {

void Walkmesh::computeAABB() {
    _aabb.reset();

    for (auto &vert : _vertices) {
        _aabb.expand(vert);
    }
}

bool Walkmesh::findObstacle(const glm::vec3 &from, const glm::vec3 &to, glm::vec3 &intersection) const {
    glm::vec3 delta(to - from);
    glm::vec3 dir(glm::normalize(delta));

    for (auto &face : _nonWalkableFaces) {
        const glm::vec3 &p0 = _vertices[face.indices[0]];
        const glm::vec3 &p1 = _vertices[face.indices[1]];
        const glm::vec3 &p2 = _vertices[face.indices[2]];

        glm::vec2 baryIntersection { 0.0f };
        float distance = 0.0f;

        if (glm::intersectRayTriangle(from, dir, p0, p1, p2, baryIntersection, distance) &&
            distance >= 0.0f &&
            distance * distance <= glm::length2(delta)) {

            intersection = from + dir * distance;
            return true;
        }
    }

    return false;
}

bool Walkmesh::findElevationAt(const glm::vec3 &position, float &z) const {
    for (auto &face : _walkableFaces) {
        const glm::vec3 &p0 = _vertices[face.indices[0]];
        const glm::vec3 &p1 = _vertices[face.indices[1]];
        const glm::vec3 &p2 = _vertices[face.indices[2]];

        glm::vec2 intersection(0.0f);
        float distance = 0.0f;

        if (glm::intersectRayTriangle(position, glm::vec3(0.0f, 0.0f, -1.0f), p0, p1, p2, intersection, distance)) {
            z = position.z - distance;
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
