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

#include "../../src/common/stream/bytearrayinput.h"
#include "../../src/common/stringbuilder.h"
#include "../../src/resource/format/bifreader.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(bif_reader)

BOOST_AUTO_TEST_CASE(should_read_bif) {
    // given

    auto input = StringBuilder()
                     // header
                     .append("BIFFV1  ")
                     .append("\x01\x00\x00\x00", 4) // number of variable resources
                     .append("\x00\x00\x00\x00", 4) // number of fixed resources
                     .append("\x14\x00\x00\x00", 4) // offset to variable resources
                     // variable resource table
                     .append("\x00\x00\x00\x00", 4) // id
                     .append("\x24\x00\x00\x00", 4) // offset
                     .append("\x0d\x00\x00\x00", 4) // filesize
                     .append("\xe6\x07\x00\x00", 4) // type
                     // variable resource data
                     .append("Hello, world!")
                     .build();

    auto reader = BifReader();
    auto inputBytes = ByteArray();
    inputBytes.insert(inputBytes.end(), input.begin(), input.end());
    auto bif = make_shared<ByteArrayInputStream>(inputBytes);
    auto expectedData = ByteArray {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};

    // when

    reader.load(bif);

    // then

    auto actualData = reader.getResourceData(0);
    BOOST_TEST((expectedData == *actualData), notEqualMessage(expectedData, *actualData));
}

BOOST_AUTO_TEST_SUITE_END()
