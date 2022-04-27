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

#include "../src/resource/format/rimreader.h"

#include "checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(rim_reader)

BOOST_AUTO_TEST_CASE(should_read_rim) {
    // given

    auto ss = ostringstream();
    // header
    ss << "RIM V1.0";
    ss << string("\x00\x00\x00\x00", 4); // reserved
    ss << string("\x01\x00\x00\x00", 4); // number of resources
    ss << string("\x78\x00\x00\x00", 4); // offset to resources
    ss << string(100, '\x00');           // reserved
    // resources
    ss << string("Aa\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16); // resref
    ss << string("\xe6\x07\x00\x00", 4);                                            // type
    ss << string("\x00\x00\x00\x00", 4);                                            // id
    ss << string("\x98\x00\x00\x00", 4);                                            // offset
    ss << string("\x02\x00\x00\x00", 4);                                            // size
    // resource data
    ss << string("Bb", 2);

    auto reader = RimReader();
    auto rim = make_shared<istringstream>(ss.str());
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
