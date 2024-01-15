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

#include <gtest/gtest.h>

#include "reone/game/pathfinder.h"

using namespace reone;
using namespace reone::game;
using namespace reone::resource;

TEST(pathfinder, should_find_shortest_path) {
    // given
    std::vector<Path::Point> points {{0.0f, 0.0f, {1, 4, 5}},
                                     {1.0f, 0.0f, {0, 2, 4, 5}},
                                     {2.0f, 0.0f, {1, 3, 6}},
                                     {3.0f, 0.0f, {2, 7}},
                                     {0.0f, 1.0f, {0, 1, 5, 8}},
                                     {1.0f, 1.0f, {0, 1, 4}},
                                     {2.0f, 1.0f, {2, 10}},
                                     {3.0f, 1.0f, {3, 11}},
                                     {0.0f, 2.0f, {4, 12}},
                                     {1.0f, 2.0f, {10, 13, 14}},
                                     {2.0f, 2.0f, {6, 9, 13, 14}},
                                     {3.0f, 2.0f, {7}},
                                     {0.0f, 3.0f, {8, 13}},
                                     {1.0f, 3.0f, {9, 10, 12, 14}},
                                     {2.0f, 3.0f, {9, 10, 13, 15}},
                                     {3.0f, 3.0f, {14}}};
    std::unordered_map<int, float> pointToZ {{0, 0.0f},
                                             {1, 0.0f},
                                             {2, 0.0f},
                                             {3, 0.0f},
                                             {4, 0.0f},
                                             {5, 0.0f},
                                             {6, 0.0f},
                                             {7, 0.0f},
                                             {8, 0.0f},
                                             {9, 0.0f},
                                             {10, 0.0f},
                                             {12, 0.0f},
                                             {12, 0.0f},
                                             {13, 0.0f},
                                             {14, 0.0f},
                                             {15, 0.0f}};

    Pathfinder pathfinder;
    pathfinder.load(points, pointToZ);

    glm::vec3 from {1.0f, 1.0f, 0.0f};
    glm::vec3 to {1.0f, 3.0f, 0.0f};

    // when
    auto path = pathfinder.findPath(from, to);

    // then
    EXPECT_EQ(path.size(), 5);
    EXPECT_EQ(path.at(0), (glm::vec3 {1.0f, 1.0f, 0.0f}));
    EXPECT_EQ(path.at(1), (glm::vec3 {0.0f, 1.0f, 0.0f}));
    EXPECT_EQ(path.at(2), (glm::vec3 {0.0f, 2.0f, 0.0f}));
    EXPECT_EQ(path.at(3), (glm::vec3 {0.0f, 3.0f, 0.0f}));
    EXPECT_EQ(path.at(4), (glm::vec3 {1.0f, 3.0f, 0.0f}));
}
