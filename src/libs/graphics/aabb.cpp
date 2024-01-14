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

void AABB::onMinMaxChanged() {
    _center = 0.5f * (_min + _max);
    _size = _max - _min;
}

AABB AABB::operator*(const glm::mat4 &m) const {
    if (_degenerate) {
        return *this;
    }
    std::array<glm::vec4, 8> corners;
    corners[0] = {_min.x, _min.y, _min.z, 1.0f};
    corners[1] = {_min.x, _min.y, _max.z, 1.0f};
    corners[2] = {_min.x, _max.y, _min.z, 1.0f};
    corners[3] = {_min.x, _max.y, _max.z, 1.0f};
    corners[4] = {_max.x, _min.y, _min.z, 1.0f};
    corners[5] = {_max.x, _min.y, _max.z, 1.0f};
    corners[6] = {_max.x, _max.y, _min.z, 1.0f};
    corners[7] = {_max.x, _max.y, _max.z, 1.0f};
    glm::vec4 min {std::numeric_limits<float>::max()};
    glm::vec4 max {std::numeric_limits<float>::min()};
    for (auto &p : corners) {
        p = m * p;
        min = glm::min(min, p);
        max = glm::max(max, p);
    }
    return AABB {min, max};
}

void AABB::reset() {
    _min = glm::vec3 {0.0f};
    _max = glm::vec3 {0.0f};
    _center = glm::vec3 {0.0f};
    _size = glm::vec3 {0.0f};
    _degenerate = true;
}

void AABB::expand(const glm::vec3 &p) {
    if (_degenerate) {
        _min.x = p.x;
        _min.y = p.y;
        _min.z = p.z;
        _max = _min;
        _degenerate = false;
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
    onMinMaxChanged();
}

void AABB::expand(const AABB &aabb) {
    if (_degenerate) {
        _min = aabb._min;
        _max = aabb._max;
        _degenerate = false;
        return;
    }
    if (aabb._min.x < _min.x) {
        _min.x = aabb._min.x;
    }
    if (aabb._min.y < _min.y) {
        _min.y = aabb._min.y;
    }
    if (aabb._min.z < _min.z) {
        _min.z = aabb._min.z;
    }
    if (aabb._max.x > _max.x) {
        _max.x = aabb._max.x;
    }
    if (aabb._max.y > _max.y) {
        _max.y = aabb._max.y;
    }
    if (aabb._max.z > _max.z) {
        _max.z = aabb._max.z;
    }
    onMinMaxChanged();
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
                   float maxDistance, float &outDistance) const {
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
