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

#include "../../src/resource/format/bifreader.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(bif_reader)

BOOST_AUTO_TEST_CASE(should_read_bif) {
    // given

    auto ss = ostringstream();
    // header
    ss << "BIFFV1  ";
    ss << string("\x01\x00\x00\x00", 4); // number of variable resources
    ss << string("\x00\x00\x00\x00", 4); // number of fixed resources
    ss << string("\x14\x00\x00\x00", 4); // offset to variable resources
    // variable resource table
    ss << string("\x00\x00\x00\x00", 4); // id
    ss << string("\x24\x00\x00\x00", 4); // offset
    ss << string("\x0d\x00\x00\x00", 4); // filesize
    ss << string("\xe6\x07\x00\x00", 4); // type
    // variable resource data
    ss << "Hello, world!";

    auto reader = BifReader();
    auto bif = make_shared<istringstream>(ss.str());
    auto expectedData = ByteArray {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};

    // when

    reader.load(bif);

    // then

    auto actualData = reader.getResourceData(0);
    BOOST_TEST((expectedData == *actualData), notEqualMessage(expectedData, *actualData));
}

BOOST_AUTO_TEST_SUITE_END()
