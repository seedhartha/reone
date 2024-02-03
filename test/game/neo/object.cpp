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

#include "../../fixtures/neogame.h"

#include "reone/game/neo/object.h"

using namespace reone::game;
using namespace reone::game::neo;

using testing::_;

class TestObject : public Object {
public:
    TestObject(ObjectId objectId,
               ObjectTag tag) :
        Object(
            objectId,
            std::move(tag),
            ObjectType::Invalid) {
    }
};

class TestSpatialObject : public SpatialObject {
public:
    TestSpatialObject(ObjectId objectId,
                      ObjectTag tag) :
        SpatialObject(
            objectId,
            std::move(tag),
            ObjectType::Invalid) {
    }
};

TEST(Object, should_be_constructed_in_created_state) {
    // given
    TestObject object {0, ""};

    // expect
    EXPECT_TRUE(object.is(ObjectState::Created));
}

TEST(Object, should_return_nullopt_for_current_action) {
    // given
    TestObject object {0, ""};

    // when
    auto action = object.currentAction();

    // then
    EXPECT_EQ(action, std::nullopt);
}

TEST(Object, should_add_action_to_action_queue) {
    // given
    TestObject object {0, ""};

    // when
    object.add(Action());

    // then
    auto action = object.currentAction();
    EXPECT_TRUE(action.has_value());
}

TEST(Object, should_apply_effect) {
    // given
    TestObject object {0, ""};

    // when
    object.apply(Effect());

    // then
}

TEST(SpatialObject, should_be_created_at_world_zero_facing_east) {
    // given
    TestSpatialObject object {0, ""};

    // expect
    EXPECT_EQ(object.position(), (glm::vec3 {0.0f}));
    EXPECT_EQ(object.facing(), 0.0f);
}

TEST(SpatialObject, should_set_position) {
    // given
    TestSpatialObject object {0, ""};

    // when
    object.setPosition({1.0f, 1.0f, 1.0f});

    // then
    EXPECT_EQ(object.position(), (glm::vec3 {1.0f, 1.0f, 1.0f}));
    auto &events = object.events();
    EXPECT_EQ(events.size(), 2);
    EXPECT_EQ(events.at(0).type, EventType::ObjectCreated);
    EXPECT_EQ(events.at(1).type, EventType::ObjectLocationChanged);
    EXPECT_EQ(events.at(1).object.objectId, object.id());
}

TEST(SpatialObject, should_set_facing) {
    // given
    TestSpatialObject object {0, ""};

    // when
    object.setFacing(glm::radians(270.0f));

    // then
    EXPECT_EQ(object.facing(), glm::radians(270.0f));
    auto &events = object.events();
    EXPECT_EQ(events.size(), 2);
    EXPECT_EQ(events.at(0).type, EventType::ObjectCreated);
    EXPECT_EQ(events.at(1).type, EventType::ObjectLocationChanged);
    EXPECT_EQ(events.at(1).object.objectId, object.id());
}

TEST(SpatialObject, should_set_facing_point) {
    // given
    TestSpatialObject object {0, ""};
    object.setPosition({1.0f, 1.0f, 1.0f});

    // when
    object.setFacingPoint(glm::vec3 {0.0f});

    // then
    EXPECT_EQ(object.facing(), glm::radians(135.0f));
    auto &events = object.events();
    EXPECT_EQ(events.size(), 3);
    EXPECT_EQ(events.at(0).type, EventType::ObjectCreated);
    EXPECT_EQ(events.at(1).type, EventType::ObjectLocationChanged);
    EXPECT_EQ(events.at(2).type, EventType::ObjectLocationChanged);
    EXPECT_EQ(events.at(2).object.objectId, object.id());
}
