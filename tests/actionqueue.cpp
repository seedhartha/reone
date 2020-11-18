/*
 * Copyright (c) 2020 The reone project contributors
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

#define BOOST_TEST_MODULE actionqueue

#include <boost/test/included/unit_test.hpp>

#include "../src/game/actionqueue.h"

using namespace std;

using namespace reone::game;

BOOST_AUTO_TEST_CASE(test_action_completion) {
    ActionQueue actionQueue;
    actionQueue.add(make_unique<Action>(ActionType::PauseConversation));
    actionQueue.add(make_unique<Action>(ActionType::ResumeConversation));

    Action *currentAction = actionQueue.currentAction();
    BOOST_TEST((currentAction && currentAction->type() == ActionType::PauseConversation));

    currentAction->complete();
    actionQueue.update();

    currentAction = actionQueue.currentAction();
    BOOST_TEST((currentAction && currentAction->type() == ActionType::ResumeConversation));

    currentAction->complete();
    actionQueue.update();

    currentAction = actionQueue.currentAction();
    BOOST_TEST(!currentAction);
}
