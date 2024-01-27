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
    TestObject(MockGame &game) :
        Object(0, "", ObjectType::Invalid, game, game) {
    }
};

class TestSpatialObject : public SpatialObject {
public:
    TestSpatialObject(MockGame &game) :
        SpatialObject(0, "", ObjectType::Invalid, game, game) {
    }
};

TEST(object, should_be_constructed_in_created_state) {
    // given
    MockGame game;
    TestObject object {game};

    // expect
    EXPECT_TRUE(object.is(ObjectState::Created));
}

TEST(object, should_return_nullopt_for_current_action) {
    // given
    MockGame game;
    TestObject object {game};

    // when
    auto action = object.currentAction();

    // then
    EXPECT_EQ(action, std::nullopt);
}

TEST(object, should_add_action_to_action_queue) {
    // given
    MockGame game;
    TestObject object {game};

    // when
    object.add(Action());

    // then
    auto action = object.currentAction();
    EXPECT_TRUE(action.has_value());
}

TEST(object, should_apply_effect) {
    // given
    MockGame game;
    TestObject object {game};

    // when
    object.apply(Effect());

    // then
}

TEST(spatial_object, should_be_created_at_world_zero_facing_east) {
    // given
    MockGame game;
    TestSpatialObject object {game};

    // expect
    EXPECT_EQ(object.position(), (glm::vec3 {0.0f}));
    EXPECT_EQ(object.facing(), 0.0f);
}

TEST(spatial_object, should_set_position) {
    // given
    MockGame game;
    std::vector<Event> events;
    ON_CALL(game, collectEvent(_)).WillByDefault([&events](auto event) {
        events.push_back(std::move(event));
    });
    TestSpatialObject object {game};

    // when
    object.setPosition({1.0f, 1.0f, 1.0f});

    // then
    auto position = object.position();
    EXPECT_EQ(position, (glm::vec3 {1.0f, 1.0f, 1.0f}));
    EXPECT_EQ(events.size(), 2);
    EXPECT_EQ(events.at(0).type, EventType::ObjectCreated);
    EXPECT_EQ(events.at(1).type, EventType::ObjectLocationChanged);
    EXPECT_EQ(events.at(1).object.objectId, object.id());
}

TEST(spatial_object, should_set_facing) {
    // given
    MockGame game;
    std::vector<Event> events;
    ON_CALL(game, collectEvent(_)).WillByDefault([&events](auto event) {
        events.push_back(std::move(event));
    });
    TestSpatialObject object {game};

    // when
    object.setFacing(glm::radians(270.0f));

    // then
    auto facing = object.facing();
    EXPECT_EQ(facing, glm::radians(270.0f));
    EXPECT_EQ(events.size(), 2);
    EXPECT_EQ(events.at(0).type, EventType::ObjectCreated);
    EXPECT_EQ(events.at(1).type, EventType::ObjectLocationChanged);
    EXPECT_EQ(events.at(1).object.objectId, object.id());
}

TEST(spatial_object, should_set_facing_point) {
    // given
    MockGame game;
    std::vector<Event> events;
    ON_CALL(game, collectEvent(_)).WillByDefault([&events](auto event) {
        events.push_back(std::move(event));
    });
    TestSpatialObject object {game};
    object.setPosition({1.0f, 1.0f, 1.0f});

    // when
    object.setFacingPoint(glm::vec3 {0.0f});

    // then
    auto facing = object.facing();
    EXPECT_EQ(facing, glm::radians(135.0f));
    EXPECT_EQ(events.size(), 3);
    EXPECT_EQ(events.at(0).type, EventType::ObjectCreated);
    EXPECT_EQ(events.at(1).type, EventType::ObjectLocationChanged);
    EXPECT_EQ(events.at(2).type, EventType::ObjectLocationChanged);
    EXPECT_EQ(events.at(2).object.objectId, object.id());
}
