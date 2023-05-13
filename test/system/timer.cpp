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

#include "reone/system/timer.h"

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_SUITE(timer)

BOOST_AUTO_TEST_CASE(should_advance_and_time_out) {
    // given
    auto timer = Timer();
    timer.setTimeout(1.0f);

    // expect
    BOOST_TEST(timer.isSet());
    BOOST_TEST(!timer.isTimedOut());

    timer.advance(0.5f);
    BOOST_TEST(timer.isSet());
    BOOST_TEST(!timer.isTimedOut());

    timer.advance(0.5f);
    BOOST_TEST(!timer.isSet());
    BOOST_TEST(timer.isTimedOut());
}

BOOST_AUTO_TEST_SUITE_END()
