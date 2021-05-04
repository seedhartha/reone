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

#include "walkmesh.h"

#include "glm/gtx/intersect.hpp"
#include "glm/gtx/norm.hpp"

using namespace std;

namespace reone {

namespace render {

void Walkmesh::computeAABB() {
    _aabb.reset();

    for (auto &face : _walkableFaces) {
        _aabb.expand(face.vertices[0]);
        _aabb.expand(face.vertices[1]);
        _aabb.expand(face.vertices[2]);
    }
    for (auto &face : _nonWalkableFaces) {
        _aabb.expand(face.vertices[0]);
        _aabb.expand(face.vertices[1]);
        _aabb.expand(face.vertices[2]);
    }
}

bool Walkmesh::raycast(const glm::vec3 &origin, const glm::vec3 &dir, bool walkable, float &distance, int &material) const {
    float minDistance = FLT_MAX;

    const vector<Face> &faces = walkable ? _walkableFaces : _nonWalkableFaces;
    glm::vec2 baryPosition(0.0f);
    float localDistance = 0.0f;

    for (auto &face : faces) {
        const glm::vec3 &p0 = face.vertices[0];
        const glm::vec3 &p1 = face.vertices[1];
        const glm::vec3 &p2 = face.vertices[2];

        if (glm::intersectRayTriangle(origin, dir, p0, p1, p2, baryPosition, localDistance) && localDistance >= 0.0f && localDistance < minDistance) {
            minDistance = localDistance;
            material = static_cast<int>(face.material);
        }
    }

    if (minDistance == FLT_MAX) return false;

    distance = minDistance;

    return true;
}

} // namespace render

} // namespace reone
