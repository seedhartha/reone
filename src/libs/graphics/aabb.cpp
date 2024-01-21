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

#include "reone/graphics/aabb.h"

namespace reone {

namespace graphics {

void AABB::updateCorners() {
    _corners.resize(8);
    _corners[0] = {_min.x, _min.y, _min.z};
    _corners[1] = {_min.x, _min.y, _max.z};
    _corners[2] = {_min.x, _max.y, _min.z};
    _corners[3] = {_min.x, _max.y, _max.z};
    _corners[4] = {_max.x, _min.y, _min.z};
    _corners[5] = {_max.x, _min.y, _max.z};
    _corners[6] = {_max.x, _max.y, _min.z};
    _corners[7] = {_max.x, _max.y, _max.z};
}

AABB AABB::operator*(const glm::mat4 &m) const {
    if (_degenerate) {
        return *this;
    }
    glm::vec3 min {std::numeric_limits<float>::max()};
    glm::vec3 max {std::numeric_limits<float>::lowest()};
    for (auto &corner : _corners) {
        glm::vec3 p = m * glm::vec4 {corner, 1.0f};
        min = glm::min(min, p);
        max = glm::max(max, p);
    }
    return AABB {min, max};
}

void AABB::reset() {
    _degenerate = true;
    _min = glm::vec3 {0.0f};
    _max = glm::vec3 {0.0f};
    _corners.clear();
}

void AABB::expand(const glm::vec3 &p) {
    if (_degenerate) {
        _degenerate = false;
        _min = p;
        _max = p;
        updateCorners();
        return;
    }
    if (p.x < _min.x) {
        _min.x = p.x;
    }
    if (p.y < _min.y) {
        _min.y = p.y;
    }
    if (p.z < _min.z) {
        _min.z = p.z;
    }
    if (p.x > _max.x) {
        _max.x = p.x;
    }
    if (p.y > _max.y) {
        _max.y = p.y;
    }
    if (p.z > _max.z) {
        _max.z = p.z;
    }
    updateCorners();
}

void AABB::expand(const AABB &aabb) {
    expand(aabb._min);
    expand(aabb._max);
}

bool AABB::contains(const glm::vec2 &p) const {
    return p.x >= _min.x && p.x <= _max.x &&
           p.y >= _min.y && p.y <= _max.y;
}

bool AABB::contains(const glm::vec3 &p) const {
    return p.x >= _min.x && p.x <= _max.x &&
           p.y >= _min.y && p.y <= _max.y &&
           p.z >= _min.z && p.z <= _max.z;
}

bool AABB::intersect(const AABB &other) const {
    return (_min.x <= other._max.x && _max.x >= other._min.x) &&
           (_min.y <= other._max.y && _max.y >= other._min.y) &&
           (_min.z <= other._max.z && _max.z >= other._min.z);
}

bool AABB::raycast(const glm::vec3 &origin,
                   const glm::vec3 &invDir,
                   float maxDistance,
                   float &outDistance) const {
    float tx1 = (_min.x - origin.x) * invDir.x;
    float tx2 = (_max.x - origin.x) * invDir.x;

    float tmin = glm::min(tx1, tx2);
    float tmax = glm::max(tx1, tx2);

    float ty1 = (_min.y - origin.y) * invDir.y;
    float ty2 = (_max.y - origin.y) * invDir.y;

    tmin = glm::max(tmin, glm::min(ty1, ty2));
    tmax = glm::min(tmax, glm::max(ty1, ty2));

    float tz1 = (_min.z - origin.z) * invDir.z;
    float tz2 = (_max.z - origin.z) * invDir.z;

    tmin = glm::max(0.0f, glm::max(tmin, glm::min(tz1, tz2)));
    tmax = glm::min(tmax, glm::max(tz1, tz2));

    if (tmax < tmin) {
        return false;
    }
    if (tmin >= maxDistance) {
        return false;
    }

    outDistance = tmin;
    return true;
}

} // namespace graphics

} // namespace reone
