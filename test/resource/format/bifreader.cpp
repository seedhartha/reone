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

#include "reone/system/stream/bytearrayinput.h"
#include "reone/system/stringbuilder.h"
#include "reone/resource/format/bifreader.h"

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

    auto stream = ByteArrayInputStream(input);
    auto reader = BifReader();

    // when

    reader.load(stream);

    // then

    auto &resources = reader.resources();
    BOOST_TEST(1ll == resources.size());
    BOOST_TEST(0 == resources[0].id);
    BOOST_TEST(36 == resources[0].offset);
    BOOST_TEST(13 == resources[0].fileSize);
    BOOST_TEST(2022 == resources[0].resType);
}

BOOST_AUTO_TEST_SUITE_END()
