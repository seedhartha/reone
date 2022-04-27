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

#include "../../src/resource/format/keyreader.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(key_reader)

BOOST_AUTO_TEST_CASE(should_read_key) {
    // given

    auto ss = ostringstream();

    // header
    ss << "KEY V1  ";
    ss << string("\x02\x00\x00\x00", 4); // number of files
    ss << string("\x02\x00\x00\x00", 4); // number of keys
    ss << string("\x40\x00\x00\x00", 4); // offset to files
    ss << string("\x5e\x00\x00\x00", 4); // offset to keys
    ss << string("\x00\x00\x00\x00", 4); // build year
    ss << string("\x00\x00\x00\x00", 4); // build day

    ss << string("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32); // reserved

    // file 0
    ss << string("\x80\x00\x00\x00", 4); // filesize
    ss << string("\x58\x00\x00\x00", 4); // filename offset
    ss << string("\x02\x00", 2);         // filename length
    ss << string("\x00\x00", 2);         // drives

    // file 1
    ss << string("\x00\x01\x00\x00", 4); // filesize
    ss << string("\x5b\x00\x00\x00", 4); // filename offset
    ss << string("\x02\x00", 2);         // filename length
    ss << string("\x00\x00", 2);         // drives

    // filenames
    ss << string("Aa\x00", 3);
    ss << string("Bb\x00", 3);

    // key 0
    ss << string("Cc\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("\xe1\x07", 2);
    ss << string("\xd3\x07\xc0\x00", 4);

    // key 1
    ss << string("Dd\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("\xf5\x07", 2);
    ss << string("\xd3\x07\xc0\x00", 4);

    auto reader = KeyReader();
    auto key = make_shared<istringstream>(ss.str());

    // when

    reader.load(key);

    auto keys = reader.keys();
    auto files = reader.files();

    // then

    BOOST_CHECK_EQUAL(2ll, files.size());
    BOOST_CHECK_EQUAL(128, files[0].fileSize);
    BOOST_CHECK_EQUAL("Aa", files[0].filename);
    BOOST_CHECK_EQUAL(256, files[1].fileSize);
    BOOST_CHECK_EQUAL("Bb", files[1].filename);
    BOOST_CHECK_EQUAL(2ll, keys.size());
    BOOST_CHECK_EQUAL("cc", keys[0].resId.resRef);
    BOOST_CHECK_EQUAL(static_cast<int>(ResourceType::TwoDa), static_cast<int>(keys[0].resId.type));
    BOOST_CHECK_EQUAL(12, keys[0].bifIdx);
    BOOST_CHECK_EQUAL(2003, keys[0].resIdx);
    BOOST_CHECK_EQUAL("dd", keys[1].resId.resRef);
    BOOST_CHECK_EQUAL(static_cast<int>(ResourceType::Gff), static_cast<int>(keys[1].resId.type));
    BOOST_CHECK_EQUAL(12, keys[1].bifIdx);
    BOOST_CHECK_EQUAL(2003, keys[1].resIdx);
}

BOOST_AUTO_TEST_SUITE_END()
