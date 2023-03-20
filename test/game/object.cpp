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

#include "../../src/game/object.h"

#include "../fixtures/functional.h"
#include "../fixtures/game.h"

using namespace reone;
using namespace reone::game;

BOOST_AUTO_TEST_SUITE(game_object)

BOOST_AUTO_TEST_CASE(should_execute_queued_actions_one_per_frame) {
    // given
    auto test = FunctionalTest();
    auto game = test.mockGame();

    auto action1 = game->mockAction(ActionType::StartConversation);
    auto action2 = game->mockAction(ActionType::OpenDoor);

    auto action3 = game->mockAction(ActionType::FollowLeader);
    action3->setCompleteOnExecute(false);

    auto creature = game->mockCreature();
    creature->enqueue(action1);
    creature->enqueue(action2);
    creature->enqueue(action3);

    // when
    creature->update(1.0f);
    creature->update(1.0f);
    creature->update(1.0f);
    creature->update(1.0f);

    // then
    BOOST_TEST(1ll == creature->actions().size());
    BOOST_TEST(static_cast<int>(ActionType::FollowLeader) == static_cast<int>(creature->actions().front()->type()));
    BOOST_TEST(1ll == action1->executeInvocations().size());
    BOOST_TEST(1ll == action2->executeInvocations().size());
    BOOST_TEST(2ll == action3->executeInvocations().size());
}

BOOST_AUTO_TEST_SUITE_END()
