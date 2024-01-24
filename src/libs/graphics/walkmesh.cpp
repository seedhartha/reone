/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/walkmesh.h"

namespace reone {

namespace graphics {

const Walkmesh::Face *Walkmesh::raycast(
    std::set<uint32_t> surfaces,
    const glm::vec3 &origin,
    const glm::vec3 &dir,
    float maxDistance,
    float &outDistance) const {

    // For area walkmeshes, find intersection via AABB tree
    if (_rootAabb) {
        return raycastAABB(surfaces, origin, dir, maxDistance, outDistance);
    }

    // For placeable and door walkmeshes, test all faces for intersection
    float distance = 0.0f;
    float minDistance = std::numeric_limits<float>::max();
    std::optional<std::reference_wrapper<const Face>> intersected;
    for (auto &face : _faces) {
        if (!raycastFace(surfaces, face, origin, dir, maxDistance, distance)) {
            continue;
        }
        if (distance < minDistance) {
            minDistance = distance;
            intersected = face;
        }
    }
    if (intersected) {
        outDistance = minDistance;
        return &intersected->get();
    }

    return nullptr;
}

const Walkmesh::Face *Walkmesh::raycastAABB(
    std::set<uint32_t> surfaces,
    const glm::vec3 &origin,
    const glm::vec3 &dir,
    float maxDistance,
    float &outDistance) const {

    float distance = 0.0f;

    std::stack<AABB *> aabbs;
    aabbs.push(_rootAabb.get());

    auto invDir = 1.0f / dir;

    while (!aabbs.empty()) {
        auto aabb = aabbs.top();
        aabbs.pop();

        // Test ray/face intersection for tree leafs
        if (aabb->faceIdx != -1) {
            const Face &face = _faces[aabb->faceIdx];
            if (raycastFace(surfaces, face, origin, dir, maxDistance, distance)) {
                outDistance = distance;
                return &face;
            }
            continue;
        }

        // Test ray/AABB intersection
        if (!aabb->value.raycast(origin, invDir, maxDistance, distance)) {
            continue;
        }

        // Find intersection with child AABB nodes
        if (aabb->left) {
            aabbs.push(aabb->left.get());
        }
        if (aabb->right) {
            aabbs.push(aabb->right.get());
        }
    }

    return nullptr;
}

bool Walkmesh::raycastFace(
    std::set<uint32_t> surfaces,
    const Face &face,
    const glm::vec3 &origin,
    const glm::vec3 &dir,
    float maxDistance,
    float &outDistance) const {

    if (surfaces.count(face.material) == 0) {
        return false;
    }

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

bool Walkmesh::contains(const glm::vec2 &point) const {
    if (!_rootAabb) {
        return false;
    }
    return _rootAabb->value.contains(point);
}

} // namespace graphics

} // namespace reone
