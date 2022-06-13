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

#include "../../fixtures/game.h"
#include "../../checkutil.h"

using namespace std;

using namespace reone::game;

BOOST_AUTO_TEST_SUITE(move_to_object_action)

BOOST_AUTO_TEST_CASE(should_execute) {
    // given
    auto creature1 = mockCreature(2);
    auto creature2 = mockCreature(2);
    auto action = MoveToObjectAction(*creature2, true, 1.0f);

    // when
    action.execute(*creature1, 1.0f);

    // then
}

BOOST_AUTO_TEST_SUITE_END()
