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

#pragma once

#include "reone/system/randomutil.h"

namespace reone {

namespace graphics {

inline glm::vec3 cartesianToBarycentric(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &p) {
    glm::vec3 v0(b - a);
    glm::vec3 v1(c - a);
    glm::vec3 v2(p - a);

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return glm::vec3(u, v, w);
}

inline glm::vec2 barycentricToCartesian(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c, const glm::vec3 &p) {
    return a * p.x + b * p.y + c * p.z;
}

inline glm::vec3 barycentricToCartesian(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &p) {
    return a * p.x + b * p.y + c * p.z;
}

inline glm::vec3 getRandomBarycentric() {
    float r1sqrt = glm::sqrt(randomFloat(0.0f, 1.0f));
    float r2 = randomFloat(0.0f, 1.0f);
    return glm::vec3(1.0f - r1sqrt, r1sqrt * (1.0f - r2), r2 * r1sqrt);
}

} // namespace graphics

} // namespace reone
