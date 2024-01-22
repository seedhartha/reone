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

#include "reone/game/neo/object.h"

using namespace reone::game;
using namespace reone::game::neo;

class TestObject : public Object {
public:
    TestObject() :
        Object(0, "", ObjectType::Invalid) {
    }
};

class TestSpatialObject : public SpatialObject {
public:
    TestSpatialObject() :
        SpatialObject(0, "", ObjectType::Invalid) {
    }
};

TEST(object, should_be_constructed_in_created_state) {
    // given
    TestObject object;

    // expect
    EXPECT_TRUE(object.is(ObjectState::Created));
}

TEST(object, should_return_nullopt_for_current_action) {
    // given
    TestObject object;

    // when
    auto action = object.currentAction();

    // then
    EXPECT_EQ(action, std::nullopt);
}

TEST(object, should_add_action_to_action_queue) {
    // given
    TestObject object;

    // when
    object.add(Action());

    // then
    auto action = object.currentAction();
    EXPECT_TRUE(action.has_value());
}

TEST(object, should_apply_effect) {
    // given
    TestObject object;

    // when
    object.apply(Effect());

    // then
}

TEST(spatial_object, should_be_created_at_world_zero_facing_east) {
    // given
    TestSpatialObject object;

    // expect
    EXPECT_EQ(object.position(), (glm::vec3 {0.0f}));
    EXPECT_EQ(object.facing(), 0.0f);
}

TEST(spatial_object, should_set_position) {
    // given
    TestSpatialObject object;

    // when
    object.setPosition({1.0f, 1.0f, 1.0f});

    // then
    auto position = object.position();
    EXPECT_EQ(position, (glm::vec3 {1.0f, 1.0f, 1.0f}));
    EXPECT_EQ(object.events().size(), 1);
    EXPECT_EQ(object.events().back().type, EventType::ObjectLocationChanged);
    EXPECT_EQ(object.events().back().object.objectId, object.id());
}

TEST(spatial_object, should_set_facing) {
    // given
    TestSpatialObject object;

    // when
    object.setFacing(glm::radians(270.0f));

    // then
    auto facing = object.facing();
    EXPECT_EQ(facing, glm::radians(270.0f));
    EXPECT_EQ(object.events().size(), 1);
    EXPECT_EQ(object.events().back().type, EventType::ObjectLocationChanged);
    EXPECT_EQ(object.events().back().object.objectId, object.id());
}

TEST(spatial_object, should_set_facing_point) {
    // given
    TestSpatialObject object;
    object.setPosition({1.0f, 1.0f, 1.0f});

    // when
    object.setFacingPoint(glm::vec3 {0.0f});

    // then
    auto facing = object.facing();
    EXPECT_EQ(facing, glm::radians(135.0f));
    EXPECT_EQ(object.events().size(), 2);
    EXPECT_EQ(object.events().back().type, EventType::ObjectLocationChanged);
    EXPECT_EQ(object.events().back().object.objectId, object.id());
}
