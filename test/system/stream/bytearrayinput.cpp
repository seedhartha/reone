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

#include "../../checkutil.h"

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_SUITE(byte_array_input_stream)

BOOST_AUTO_TEST_CASE(should_read_from_byte_array) {
    // given
    auto bytes = ByteArray("Hello, world!");
    auto stream = ByteArrayInputStream(bytes);
    auto buf = ByteArray(16, '\0');
    auto expectedContents = string("Hello, world!");

    // when
    stream.seek(0, SeekOrigin::End);
    size_t position1 = stream.position();
    stream.seek(0, SeekOrigin::Begin);
    int readByteResult1 = stream.readByte();
    stream.seek(-1, SeekOrigin::Current);
    size_t position2 = stream.position();
    int readResult = stream.read(&buf[0], 16);
    auto contents = buf.substr(0, 13);
    int readByteResult2 = stream.readByte();
    bool eof = stream.eof();

    // then
    BOOST_TEST(13ll == position1);
    BOOST_TEST('H' == readByteResult1);
    BOOST_TEST(0ll == position2);
    BOOST_TEST(13 == readResult);
    BOOST_TEST(16ll == buf.size());
    BOOST_TEST((expectedContents == contents), notEqualMessage(expectedContents, contents));
    BOOST_TEST(-1 == readByteResult2);
    BOOST_TEST(true == eof);
}

BOOST_AUTO_TEST_SUITE_END()
