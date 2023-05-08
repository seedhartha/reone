/*
 * Copyright (c) 2020-2022 The reone project contributors
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

namespace game {

struct Path {
    struct Point {
        glm::vec2 coords {0.0f};
        std::vector<int> adjPoints;
    };

    std::vector<Point> points;

    int indexOfClosestPoint(const glm::vec2 &to) const {
        int closestIdx = -1;
        float closestDist = std::numeric_limits<float>::max();
        for (int i = 0; i < static_cast<int>(points.size()); ++i) {
            float dist = glm::distance2(points[i].coords, to);
            if (dist < closestDist) {
                closestIdx = i;
                closestDist = dist;
            }
        }
        return closestIdx;
    }

    float squareDistanceBetween(int leftIdx, int rightIdx) const {
        return glm::distance2(points[leftIdx].coords, points[rightIdx].coords);
    }
};

} // namespace game

} // namespace reone
