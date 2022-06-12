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

#include <boost/test/unit_test.hpp>

#include "../../src/game/astar.h"
#include "../../src/game/path.h"

#include "../checkutil.h"

using namespace std;

using namespace reone::game;

BOOST_AUTO_TEST_SUITE(a_star)

BOOST_AUTO_TEST_CASE(should_find_path) {
    // Points:
    // 0 1 2 3
    // 4 5 6 7
    // 8 9 10 11
    //
    // Expected path:
    // 0 x 4 5
    // 1 x 3 6
    // x 2 8 7

    // given
    auto path = Path();
    path.points.push_back(Path::Point {glm::vec2(-1.0f, -1.0f), vector<int> {1, 4}}); // 0
    path.points.push_back(Path::Point {glm::vec2(0.0f, -1.0f), vector<int> {0, 5}});  // 1
    path.points.push_back(Path::Point {glm::vec2(1.0f, -1.0f), vector<int> {3, 6}});  // 2
    path.points.push_back(Path::Point {glm::vec2(2.0f, -1.0f), vector<int> {2, 7}});  // 3
    path.points.push_back(Path::Point {glm::vec2(-1.0f, 0.0f), vector<int> {0, 9}});  // 4
    path.points.push_back(Path::Point {glm::vec2(0.0f, 0.0f), vector<int> {1}});      // 5
    path.points.push_back(Path::Point {glm::vec2(1.0f, 0.0f), vector<int> {2, 9}});   // 6
    path.points.push_back(Path::Point {glm::vec2(2.0f, 0.0f), vector<int> {3, 11}});  // 7
    path.points.push_back(Path::Point {glm::vec2(-1.0f, 1.0f), vector<int> {}});      // 8
    path.points.push_back(Path::Point {glm::vec2(0.0f, 1.0f), vector<int> {4, 6}});   // 9
    path.points.push_back(Path::Point {glm::vec2(1.0f, 1.0f), vector<int> {11}});     // 10
    path.points.push_back(Path::Point {glm::vec2(2.0f, 1.0f), vector<int> {7, 10}});  // 11

    auto aStar = AStar(path);
    auto expectedPath = vector<glm::vec2> {
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, -1.0f),
        glm::vec2(2.0f, -1.0f),
        glm::vec2(2.0f, 0.0f),
        glm::vec2(2.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.6f)};

    // when
    auto actualPath = aStar.plotPath(glm::vec2(-0.6f, -1.0f), glm::vec2(1.0f, 0.6f));

    // then
    BOOST_CHECK_EQUAL(expectedPath.size(), actualPath.size());
    for (auto i = 0; i < expectedPath.size() && i < actualPath.size(); ++i) {
        BOOST_TEST((expectedPath[i] == actualPath[i]), notEqualMessage(expectedPath[i], actualPath[i]));
    }
}

BOOST_AUTO_TEST_SUITE_END()
