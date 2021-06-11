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

bool Walkmesh::raycastWalkableFirst(const glm::vec3 &origin, const glm::vec3 &dir, float &distance, int &material) const {
    glm::vec2 baryPosition(0.0f);
    float tempDistance = 0.0f;

    for (auto &face : _walkableFaces) {
        const glm::vec3 &p0 = face.vertices[0];
        const glm::vec3 &p1 = face.vertices[1];
        const glm::vec3 &p2 = face.vertices[2];

        if (glm::intersectRayTriangle(origin, dir, p0, p1, p2, baryPosition, tempDistance) && tempDistance > 0.0f) {
            distance = tempDistance;
            material = static_cast<int>(face.material);
            return true;
        }
    }

    return false;
}

bool Walkmesh::raycastNonWalkableFirst(const glm::vec3 &origin, const glm::vec3 &dir, float &distance, glm::vec3 &normal) const {
    glm::vec2 baryPosition(0.0f);
    float tempDistance = 0.0f;

    for (auto &face : _nonWalkableFaces) {
        const glm::vec3 &p0 = face.vertices[0];
        const glm::vec3 &p1 = face.vertices[1];
        const glm::vec3 &p2 = face.vertices[2];

        if (glm::intersectRayTriangle(origin, dir, p0, p1, p2, baryPosition, tempDistance) && tempDistance > 0.0f) {
            distance = tempDistance;
            normal = face.normal;
            return true;
        }
    }

    return false;
}

bool Walkmesh::raycastNonWalkableClosest(const glm::vec3 &origin, const glm::vec3 &dir, float &distance, glm::vec3 &normal) const {
    distance = numeric_limits<float>::max();

    glm::vec2 baryPosition(0.0f);
    float tempDistance = 0.0f;

    for (auto &face : _nonWalkableFaces) {
        const glm::vec3 &p0 = face.vertices[0];
        const glm::vec3 &p1 = face.vertices[1];
        const glm::vec3 &p2 = face.vertices[2];

        if (glm::intersectRayTriangle(origin, dir, p0, p1, p2, baryPosition, tempDistance) && tempDistance > 0.0f && tempDistance < distance) {
            distance = tempDistance;
            normal = face.normal;
        }
    }

    return distance != numeric_limits<float>::max();
}

} // namespace graphics

} // namespace reone
