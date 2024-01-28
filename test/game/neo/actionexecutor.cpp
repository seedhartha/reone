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
#include "../../fixtures/resource.h"
#include "../../fixtures/scene.h"

#include "reone/game/neo/action.h"
#include "reone/game/neo/actionexecutor.h"
#include "reone/game/neo/object/creature.h"
#include "reone/scene/collision.h"

using namespace reone::game;
using namespace reone::game::neo;
using namespace reone::resource;
using namespace reone::scene;

using testing::_;
using testing::Return;
using testing::ReturnRef;
using testing::Test;

class ActionExecutorFixture : public Test {
protected:
    std::mutex _sceneMutex;
    std::unique_lock<std::mutex> _sceneLock {_sceneMutex, std::defer_lock};
    TestResourceModule _resourceModule;
    TestSceneModule _sceneModule;
    MockSceneGraph _sceneGraph;
    MockEventCollector _eventCollector;
    Module _module {0, "", _eventCollector};

    std::unique_ptr<ActionExecutor> _subject;

    void SetUp() override {
        _resourceModule.init();
        _sceneModule.init();

        ON_CALL(_sceneModule.graphs(), get(_)).WillByDefault(ReturnRef(_sceneGraph));

        _subject = std::make_unique<ActionExecutor>(
            _sceneLock,
            _resourceModule.services(),
            _sceneModule.services());
        _subject->setModule(_module);
    }
};

TEST_F(ActionExecutorFixture, should_complete_move_to_point_action_given_less_than_min_dist_to_dest) {
    // given
    MockEventCollector eventCollector;
    Creature creature {1, "", eventCollector};
    Action moveToPoint;
    moveToPoint.type = ActionType::MoveToPoint;
    moveToPoint.location.position = glm::vec3 {0.05f, 0.0f, 0.0f};

    // when
    bool completed = _subject->executeAction(creature, moveToPoint, 1.0f);

    // then
    EXPECT_TRUE(completed);
    EXPECT_EQ(creature.moveType(), Creature::MoveType::None);
    EXPECT_EQ(creature.position(), glm::vec3 {0.0f});
    EXPECT_EQ(creature.facing(), 0.0f);
}

TEST_F(ActionExecutorFixture, should_not_move_subject_given_move_to_point_action_and_failed_walk_test) {
    // given
    MockEventCollector eventCollector;
    Creature creature {1, "", eventCollector};
    Action moveToPoint;
    moveToPoint.type = ActionType::MoveToPoint;
    moveToPoint.location.position = glm::vec3 {1.0f, 0.0f, 0.0f};
    EXPECT_CALL(_sceneGraph, testWalk(_, _, _, _)).WillOnce(Return(true));

    // when
    bool completed = _subject->executeAction(creature, moveToPoint, 1.0f);

    // then
    EXPECT_FALSE(completed);
    EXPECT_EQ(creature.moveType(), Creature::MoveType::None);
    EXPECT_EQ(creature.position(), glm::vec3 {0.0f});
    EXPECT_NEAR(creature.facing(), -glm::half_pi<float>(), 1e-4f);
}

TEST_F(ActionExecutorFixture, should_not_move_subject_given_move_to_point_action_and_failed_elevation_test) {
    // given
    MockEventCollector eventCollector;
    Creature creature {1, "", eventCollector};
    Action moveToPoint;
    moveToPoint.type = ActionType::MoveToPoint;
    moveToPoint.location.position = glm::vec3 {1.0f, 0.0f, 0.0f};
    EXPECT_CALL(_sceneGraph, testWalk(_, _, _, _)).WillOnce(Return(false));
    EXPECT_CALL(_sceneGraph, testElevation(_, _)).WillOnce(Return(false));

    // when
    bool completed = _subject->executeAction(creature, moveToPoint, 1.0f);

    // then
    EXPECT_FALSE(completed);
    EXPECT_EQ(creature.moveType(), Creature::MoveType::None);
    EXPECT_EQ(creature.position(), glm::vec3 {0.0f});
    EXPECT_NEAR(creature.facing(), -glm::half_pi<float>(), 1e-4f);
}

TEST_F(ActionExecutorFixture, should_move_subject_given_move_to_point_action) {
    // given
    MockEventCollector eventCollector;
    Creature creature {1, "", eventCollector};
    Action moveToPoint;
    moveToPoint.type = ActionType::MoveToPoint;
    moveToPoint.location.position = glm::vec3 {1.0f, 0.0f, 0.0f};
    EXPECT_CALL(_sceneGraph, testWalk(_, _, _, _)).WillOnce(Return(false));
    EXPECT_CALL(_sceneGraph, testElevation(_, _)).WillOnce([](const glm::vec2 &position, Collision &collision) {
        collision.intersection = glm::vec3 {1.0f, 0.0f, -1.0f};
        return true;
    });

    // when
    bool completed = _subject->executeAction(creature, moveToPoint, 1.0f);

    // then
    EXPECT_FALSE(completed);
    EXPECT_EQ(creature.moveType(), Creature::MoveType::Run);
    EXPECT_EQ(creature.position(), (glm::vec3 {1.0f, 0.0f, -1.0f}));
    EXPECT_NEAR(creature.facing(), -glm::half_pi<float>(), 1e-4f);
}
