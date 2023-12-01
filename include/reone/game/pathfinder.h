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

#include "reone/resource/path.h"

namespace reone {

namespace game {

/**
 * A* pathfinding.
 */
class Pathfinder : boost::noncopyable {
public:
    void load(const std::vector<resource::Path::Point> &points, const std::unordered_map<int, float> &pointZ);

    const std::vector<glm::vec3> findPath(const glm::vec3 &from, const glm::vec3 &to) const;

private:
    struct ContextVertex {
        uint16_t index {0};
        uint16_t parentIndex {0xffff};
        float distance {0.0f};
        float heuristic {0.0f};
        float totalCost {0.0f};
    };

    struct Context {
        std::unordered_map<uint16_t, ContextVertex> vertices;
        std::set<uint16_t> open;
        std::set<uint16_t> closed;

        const ContextVertex &getVertexWithLeastTotalCostFromOpen() const;
    };

    std::vector<glm::vec3> _vertices;
    std::unordered_map<uint16_t, std::vector<uint16_t>> _adjacentVertices;

    uint16_t getNearestVertex(const glm::vec3 &point) const;
};

} // namespace game

} // namespace reone
