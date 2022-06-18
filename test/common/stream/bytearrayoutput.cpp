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

#include "../../../src/common/stream/bytearrayoutput.h"

#include "../../checkutil.h"

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_SUITE(byte_array_output_stream)

BOOST_AUTO_TEST_CASE(should_write_to_byte_array) {
    // given
    auto bytes = ByteArray();
    auto stream = ByteArrayOutputStream(bytes);
    auto bytesToWrite = ByteArray("Hello, world!");
    auto expectedOutput = ByteArray("Hello, world!\nHello, world!");

    // when
    stream.write(bytesToWrite);
    auto position = stream.position();
    stream.writeByte('\n');
    stream.writeByte('H');
    stream.write(&bytesToWrite[1], 12);

    // then
    BOOST_CHECK_EQUAL(13ll, position);
    BOOST_TEST((expectedOutput == bytes), notEqualMessage(expectedOutput, bytes));
}

BOOST_AUTO_TEST_SUITE_END()