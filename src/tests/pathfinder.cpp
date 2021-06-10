/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#define BOOST_TEST_MODULE pathfinder

#include <boost/test/included/unit_test.hpp>

#include "../engine/game/path.h"
#include "../engine/game/pathfinder.h"

using namespace std;

using namespace reone::game;
using namespace reone::resource;

BOOST_AUTO_TEST_CASE(test_find_path) {
    vector<Path::Point> points = {
        { 1.0f, 1.0f, { 1, 2 } },
        { 1.0f, 2.0f, { 0, 2 } },
        { 2.0f, 2.0f, { 1, 3, 4 } },
        { 3.0f, 2.0f, { 2, 4 } },
        { 3.0f, 3.0f, { 2, 3 } }
    };
    unordered_map<int, float> pointZ;
    for (int i = 0; i < points.size(); ++i) {
        pointZ.insert(make_pair(i, 0.0f));
    }
    Pathfinder pathfinder;
    pathfinder.load(points, pointZ);

    glm::vec3 from(glm::vec3(1.0f, 1.0f, 0.0f));
    glm::vec3 to(glm::vec3(3.0f, 3.0f, 0.0f));
    vector<glm::vec3> path(pathfinder.findPath(from, to));

    bool found =
        path.size() == 3 &&
        path[0] == glm::vec3(1.0f, 1.0f, 0.0f) &&
        path[1] == glm::vec3(2.0f, 2.0f, 0.0f) &&
        path[2] == glm::vec3(3.0f, 3.0f, 0.0f);

    BOOST_TEST(found);
}

BOOST_AUTO_TEST_CASE(find_path_when_no_points_then_from_to_returned) {
    Pathfinder pathfinder;
    pathfinder.load({}, {});

    glm::vec3 from(glm::vec3(1.0f, 1.0f, 0.0f));
    glm::vec3 to(glm::vec3(3.0f, 3.0f, 0.0f));
    vector<glm::vec3> path(pathfinder.findPath(from, to));

    bool found =
        path.size() == 2 &&
        path[0] == glm::vec3(1.0f, 1.0f, 0.0f) &&
        path[1] == glm::vec3(3.0f, 3.0f, 0.0f);

    BOOST_TEST(found);
}
