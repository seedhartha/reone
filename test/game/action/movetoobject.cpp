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

#include "../../../src/game/action/movetoobject.h"

#include "../../checkutil.h"
#include "../../fixtures/functional.h"
#include "../../fixtures/game.h"

using namespace std;

using namespace reone;
using namespace reone::game;

BOOST_AUTO_TEST_SUITE(move_to_object_action)

BOOST_AUTO_TEST_CASE(should_complete_when_destination_is_reached) {
    // given
    auto test = FunctionalTest();
    auto game = test.mockGame();

    auto creature1 = game->mockCreature();
    creature1->whenMoveToThenReturn(true);
    auto creature2 = game->mockCreature();
    auto action = MoveToObjectAction(*creature2, true, 1.0f);

    // when
    action.execute(*creature1, 1.0f);

    // then
    BOOST_TEST(1ll == creature1->moveToInvocations().size());
    BOOST_TEST(action.isCompleted());
}

BOOST_AUTO_TEST_CASE(should_not_complete_when_destination_is_not_reached) {
    // given
    auto test = FunctionalTest();
    auto game = test.mockGame();

    auto creature1 = game->mockCreature();
    creature1->whenMoveToThenReturn(false);
    auto creature2 = game->mockCreature();
    auto action = MoveToObjectAction(*creature2, true, 1.0f);

    // when
    action.execute(*creature1, 1.0f);

    // then
    BOOST_TEST(1ll == creature1->moveToInvocations().size());
    BOOST_TEST(!action.isCompleted());
}

BOOST_AUTO_TEST_SUITE_END()
