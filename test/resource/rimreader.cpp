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
#include "../../src/resource/format/rimreader.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(rim_reader)

BOOST_AUTO_TEST_CASE(should_read_rim) {
    // given

    auto input = StringBuilder()
                     // header
                     .append("RIM V1.0")
                     .append("\x00\x00\x00\x00", 4) // reserved
                     .append("\x01\x00\x00\x00", 4) // number of resources
                     .append("\x78\x00\x00\x00", 4) // offset to resources
                     .repeat('\x00', 100)           // reserved
                     // resources
                     .append("Aa\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) // resref
                     .append("\xe6\x07\x00\x00", 4)                                            // type
                     .append("\x00\x00\x00\x00", 4)                                            // id
                     .append("\x98\x00\x00\x00", 4)                                            // offset
                     .append("\x02\x00\x00\x00", 4)                                            // size
                     // resource data
                     .append("Bb", 2)
                     .build();

    auto reader = RimReader();

    auto inputBytes = ByteArray();
    inputBytes.insert(inputBytes.end(), input.begin(), input.end());
    auto rim = make_shared<ByteArrayInputStream>(inputBytes);

    auto expectedData = ByteArray {'B', 'b'};

    // when

    reader.load(rim);

    // then

    auto resources = reader.resources();
    BOOST_CHECK_EQUAL(1ll, resources.size());
    BOOST_CHECK_EQUAL("aa", resources.front().resId.resRef);
    BOOST_CHECK_EQUAL(static_cast<int>(ResourceType::Txi), static_cast<int>(resources.front().resId.type));
    BOOST_CHECK_EQUAL(152, resources.front().offset);
    BOOST_CHECK_EQUAL(2, resources.front().size);
    auto actualData = reader.getResourceData(0);
    BOOST_TEST((expectedData == actualData), notEqualMessage(expectedData, actualData));
}

BOOST_AUTO_TEST_SUITE_END()
