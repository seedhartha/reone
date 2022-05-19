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

#include "../../src/common/unicodeutil.h"

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_SUITE(unicode_utils)

BOOST_AUTO_TEST_CASE(should_convert_utf8_to_utf16) {
    // given
    auto expected = u16string(u"Hello, world!");

    // when
    auto actual = convertUTF8ToUTF16(string("Hello, world!"));

    // then
    BOOST_CHECK(expected == actual);
}

BOOST_AUTO_TEST_CASE(should_convert_utf16_to_utf8) {
    // given
    auto expected = string("Hello, world!");

    // when
    auto actual = convertUTF16ToUTF8(u16string(u"Hello, world!"));

    // then
    BOOST_CHECK_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_SUITE_END()
