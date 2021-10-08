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

/** @file
 *  Tests for Unicode conversion functions.
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../../src/engine/common/unicodeutil.h"

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_CASE(WhenConvertUTF8ToUTF16) {
    string utf8("\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82\x2c\x20\xd0\xbc\xd0\xb8\xd1\x80\x21");
    u16string utf16(convertUTF8ToUTF16(utf8));
    BOOST_TEST((utf16 == u"\u041f\u0440\u0438\u0432\u0435\u0442\u002c\u0020\u043c\u0438\u0440\u0021"));
}

BOOST_AUTO_TEST_CASE(WhenConvertUTF16ToUTF8) {
    u16string utf16(u"\u041f\u0440\u0438\u0432\u0435\u0442\u002c\u0020\u043c\u0438\u0440\u0021");
    string utf8(convertUTF16ToUTF8(utf16));
    BOOST_TEST((utf8 == "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82\x2c\x20\xd0\xbc\xd0\xb8\xd1\x80\x21"));
}
