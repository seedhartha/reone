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

#pragma once

#include <cmath>

#include "glm/vec3.hpp"

namespace reone {

struct Vector3 {
    float x { 0.0f };
    float y { 0.0f };
    float z { 0.0f };

    Vector3() = default;

    Vector3(float x, float y, float z) : x(x), y(y), z(z) {
    }

    Vector3(const glm::vec3 &other) : x(other.x), y(other.y), z(other.z) {
    }

    bool operator==(const Vector3 &other) {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vector3 &other) {
        return !operator==(other);
    }

    void normalize() {
        float len = length();
        if (len < 1e-6) return;

        float invLen = 1.0f / len;
        x *= invLen;
        y *= invLen;
        z *= invLen;
    }

    float length() const {
        return sqrtf(x * x + y * y + z * z);
    }
};

} // namespace reone
