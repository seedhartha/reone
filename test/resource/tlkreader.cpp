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

#include "../../src/resource/format/tlkreader.h"
#include "../../src/resource/talktable.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(tlk_reader)

BOOST_AUTO_TEST_CASE(should_read_tlk) {
    // given

    auto ss = ostringstream();
    // header
    ss << string("TLK V3.0", 8);
    ss << string("\x00\x00\x00\x00", 4); // language id
    ss << string("\x02\x00\x00\x00", 4); // number of strings
    ss << string("\x64\x00\x00\x00", 4); // offset to string entries
    // string data 0
    ss << string("\x07\x00\x00\x00", 4);                                                  // flags
    ss << string("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16); // sound res ref
    ss << string("\x00\x00\x00\x00", 4);                                                  // volume variance
    ss << string("\x00\x00\x00\x00", 4);                                                  // pitch variance
    ss << string("\x00\x00\x00\x00", 4);                                                  // offset to string
    ss << string("\x04\x00\x00\x00", 4);                                                  // string size
    ss << string("\x00\x00\x00\x00", 4);                                                  // sound length
    // string data 1
    ss << string("\x07\x00\x00\x00", 4);                                      // flags
    ss << string("jane\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16); // sound res ref
    ss << string("\x00\x00\x00\x00", 4);                                      // volume variance
    ss << string("\x00\x00\x00\x00", 4);                                      // pitch variance
    ss << string("\x04\x00\x00\x00", 4);                                      // offset to string
    ss << string("\x04\x00\x00\x00", 4);                                      // string size
    ss << string("\x00\x00\x00\x00", 4);                                      // sound length
    // string entries
    ss << "John";
    ss << "Jane";

    auto reader = TlkReader();
    auto stream = make_shared<istringstream>(ss.str());

    // when

    reader.load(stream);

    // then

    auto table = reader.table();
    BOOST_CHECK_EQUAL(2, table->getStringCount());
    BOOST_CHECK_EQUAL("John", table->getString(0).text);
    BOOST_CHECK_EQUAL("", table->getString(0).soundResRef);
    BOOST_CHECK_EQUAL("Jane", table->getString(1).text);
    BOOST_CHECK_EQUAL("jane", table->getString(1).soundResRef);
}

BOOST_AUTO_TEST_SUITE_END()
