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

#include <boost/test/unit_test.hpp>

#include "reone/graphics/aabb.h"

using namespace std;

using namespace reone;
using namespace reone::graphics;

BOOST_AUTO_TEST_SUITE(aabb)

BOOST_AUTO_TEST_CASE(should_find_ray_aabb_intersection__intersection_from_within) {
    // given
    auto aabb = AABB(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(3.0f, 2.0f, 1.0f));

    // when
    float distance = -1.0f;
    bool intersected = aabb.raycast(glm::vec3(0.0f), 1.0f / glm::vec3(1.0f, 0.0f, 0.0f), numeric_limits<float>::max(), distance);

    // then
    BOOST_REQUIRE(intersected);
    BOOST_CHECK_CLOSE(0.0f, distance, 1e-5);
}

BOOST_AUTO_TEST_CASE(should_find_ray_aabb_intersection__intersection_from_up_close) {
    // given
    auto aabb = AABB(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(3.0f, 2.0f, 1.0f));

    // when
    float distance = -1.0f;
    bool intersected = aabb.raycast(glm::vec3(-2.0f, 0.0f, 0.0f), 1.0f / glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, distance);

    // then
    BOOST_REQUIRE(intersected);
    BOOST_CHECK_CLOSE(1.0f, distance, 1e-5);
}

BOOST_AUTO_TEST_CASE(should_find_ray_aabb_intersection__intersection_from_too_far) {
    // given
    auto aabb = AABB(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(3.0f, 2.0f, 1.0f));

    // when
    float distance = -1.0f;
    bool intersected = aabb.raycast(glm::vec3(-20.0f, 0.0f, 0.0f), 1.0f / glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, distance);

    // then
    BOOST_REQUIRE(!intersected);
}

BOOST_AUTO_TEST_CASE(should_find_ray_aabb_intersection__no_intersection) {
    // given
    auto aabb = AABB(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(3.0f, 2.0f, 1.0f));

    // when
    float distance = -1.0f;
    bool intersected = aabb.raycast(glm::vec3(-2.0f, 0.0f, 0.0f), 1.0f / glm::vec3(0.0f, 0.0f, 1.0f), 10.0f, distance);

    // then
    BOOST_REQUIRE(!intersected);
}

BOOST_AUTO_TEST_SUITE_END()
