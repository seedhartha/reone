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

#include "reone/graphics/aabb.h"

using namespace reone;
using namespace reone::graphics;

TEST(AABB, should_find_ray_aabb_intersection__intersection_from_within) {
    // given
    auto aabb = AABB(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(3.0f, 2.0f, 1.0f));

    // when
    float distance = -1.0f;
    bool intersected = aabb.raycast(glm::vec3(0.0f), 1.0f / glm::vec3(1.0f, 0.0f, 0.0f), std::numeric_limits<float>::max(), distance);

    // then
    EXPECT_TRUE(intersected);
    EXPECT_NEAR(0.0f, distance, 1e-5);
}

TEST(AABB, should_find_ray_aabb_intersection__intersection_from_up_close) {
    // given
    auto aabb = AABB(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(3.0f, 2.0f, 1.0f));

    // when
    float distance = -1.0f;
    bool intersected = aabb.raycast(glm::vec3(-2.0f, 0.0f, 0.0f), 1.0f / glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, distance);

    // then
    EXPECT_TRUE(intersected);
    EXPECT_NEAR(1.0f, distance, 1e-5);
}

TEST(AABB, should_find_ray_aabb_intersection__intersection_from_too_far) {
    // given
    auto aabb = AABB(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(3.0f, 2.0f, 1.0f));

    // when
    float distance = -1.0f;
    bool intersected = aabb.raycast(glm::vec3(-20.0f, 0.0f, 0.0f), 1.0f / glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, distance);

    // then
    EXPECT_TRUE(!intersected);
}

TEST(AABB, should_find_ray_aabb_intersection__no_intersection) {
    // given
    auto aabb = AABB(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(3.0f, 2.0f, 1.0f));

    // when
    float distance = -1.0f;
    bool intersected = aabb.raycast(glm::vec3(-2.0f, 0.0f, 0.0f), 1.0f / glm::vec3(0.0f, 0.0f, 1.0f), 10.0f, distance);

    // then
    EXPECT_TRUE(!intersected);
}
