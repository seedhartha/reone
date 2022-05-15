/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "../../src/common/stringbuilder.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_SUITE(string_builder)

BOOST_AUTO_TEST_CASE(should_build_a_string) {
    // given
    auto expectedStr = string("Hello, world!");

    // when
    auto str = StringBuilder()
        .append("He")
        .repeat('l', 2)
        .append("o, world!")
        .build();

    // then
    BOOST_TEST((expectedStr == str), notEqualMessage(expectedStr, str));
}

BOOST_AUTO_TEST_SUITE_END()