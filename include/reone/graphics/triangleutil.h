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

namespace reone {

namespace graphics {

inline glm::vec3 calculateTriangleNormal(const std::vector<glm::vec3> &verts) {
    auto u = verts[1] - verts[0];
    auto v = verts[2] - verts[0];
    return glm::vec3(
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x);
}

inline float calculateTriangleArea(const std::vector<glm::vec3> &verts) {
    float a = glm::distance(verts[0], verts[1]);
    float b = glm::distance(verts[0], verts[2]);
    float c = glm::distance(verts[1], verts[2]);
    return 0.25f * glm::sqrt((a + b + c) * (-a + b + c) * (a - b + c) * (a + b - c));
}

} // namespace graphics

} // namespace reone
