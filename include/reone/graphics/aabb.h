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

class AABB {
public:
    AABB() = default;

    AABB(glm::vec3 min, glm::vec3 max) :
        _degenerate(false),
        _min(std::move(min)),
        _max(std::move(max)) {
        updateCorners();
    }

    AABB operator*(const glm::mat4 &m) const;

    void reset();
    void expand(const glm::vec3 &p);
    void expand(const AABB &aabb);

    bool contains(const glm::vec2 &point) const;
    bool contains(const glm::vec3 &point) const;
    bool intersect(const AABB &other) const;

    bool raycast(const glm::vec3 &origin,
                 const glm::vec3 &invDir,
                 float maxDistance,
                 float &outDistance) const;

    bool isDegenerate() const { return _degenerate; }

    const glm::vec3 &min() const { return _min; }
    const glm::vec3 &max() const { return _max; }
    const std::vector<glm::vec3> &corners() const { return _corners; }

private:
    bool _degenerate {true};
    glm::vec3 _min {0.0f};
    glm::vec3 _max {0.0f};

    std::vector<glm::vec3> _corners;

    void updateCorners();
};

} // namespace graphics

} // namespace reone
