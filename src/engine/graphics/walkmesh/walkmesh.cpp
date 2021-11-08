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

using namespace std;

namespace reone {

namespace graphics {

const Walkmesh::Face *Walkmesh::raycast(const glm::vec3 &origin, const glm::vec3 &dir, float maxDistance, float &outDistance) const {
    // For area walkmeshes, find intersection via AABB tree
    if (_rootAabb) {
        return raycastAABB(*_rootAabb, origin, dir, maxDistance, outDistance);
    }

    // For placeable and door walkmeshes, test all faces for intersection
    glm::vec2 baryPosition(0.0f);
    float distance = 0.0f;
    for (auto &face : _faces) {
        if (raycastFace(face, origin, dir, maxDistance, distance)) {
            outDistance = distance;
            return &face;
        }
    }

    return nullptr;
}

const Walkmesh::Face *Walkmesh::raycastAABB(AABB &aabb, const glm::vec3 &origin, const glm::vec3 &dir, float maxDistance, float &outDistance) const {
    float distance = 0.0f;

    // Find ray/AABB intersection
    if (!aabb.value.raycast(origin, dir, maxDistance, distance)) {
        return nullptr;
    }

    // For AABB tree leafs, find ray/face intersection
    if (aabb.faceIdx != -1) {
        const Face &face = _faces[aabb.faceIdx];
        if (raycastFace(face, origin, dir, maxDistance, distance)) {
            outDistance = distance;
            return &face;
        }
        return nullptr;
    }

    // Otherwise, find intersection in child AABB nodes
    if (aabb.child1) {
        auto face = raycastAABB(*aabb.child1, origin, dir, maxDistance, distance);
        if (face) {
            outDistance = distance;
            return face;
        }
    }
    if (aabb.child2) {
        auto face = raycastAABB(*aabb.child2, origin, dir, maxDistance, distance);
        if (face) {
            outDistance = distance;
            return face;
        }
    }

    return nullptr;
}

bool Walkmesh::raycastFace(const Face &face, const glm::vec3 &origin, const glm::vec3 &dir, float maxDistance, float &outDistance) const {
    const glm::vec3 &p0 = face.vertices[0];
    const glm::vec3 &p1 = face.vertices[1];
    const glm::vec3 &p2 = face.vertices[2];

    glm::vec2 baryPosition(0.0f);
    float distance = 0.0f;

    if (glm::intersectRayTriangle(origin, dir, p0, p1, p2, baryPosition, distance) && distance > 0.0f && distance < maxDistance) {
        outDistance = distance;
        return true;
    }

    return false;
}

} // namespace graphics

} // namespace reone
