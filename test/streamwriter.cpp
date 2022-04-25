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

#include "../src/common/streamwriter.h"

#include "checkutil.h"

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_SUITE(stream_writer)

BOOST_AUTO_TEST_CASE(should_write_to_little_endian_stream) {
    // given
    auto stream = make_shared<ostringstream>();
    auto writer = StreamWriter(stream, boost::endian::order::little);
    auto ss = stringstream();
    ss << string("\x40", 1);
    ss << string("\x41", 1);
    ss << string("\x01\x00", 2);
    ss << string("\x02\x00\x00\x00", 4);
    ss << string("\xfd\xff", 2);
    ss << string("\xfc\xff\xff\xff", 4);
    ss << string("\xfb\xff\xff\xff\xff\xff\xff\xff", 8);
    ss << string("\x00\x00\x80\x3f", 4);
    ss << string("AaBbCc\x00", 7);
    ss << string("\x01\x02\x03\x04", 4);
    auto expectedOutput = ss.str();

    // when
    writer.putByte(0x40);
    writer.putChar('A');
    writer.putUint16(1);
    writer.putUint32(2);
    writer.putInt16(-3);
    writer.putInt32(-4);
    writer.putInt64(-5);
    writer.putFloat(1.0f);
    writer.putString(string("Aa"));
    writer.putStringExact(string("Bb"), 2);
    writer.putCString(string("Cc\x00"));
    writer.putBytes(ByteArray {0x01, 0x02, 0x03, 0x04});

    // then
    auto output = stream->str();
    BOOST_TEST((expectedOutput == output), notEqualMessage(expectedOutput, output));
}

BOOST_AUTO_TEST_CASE(should_write_to_big_endian_stream) {
    // given
    auto stream = make_shared<ostringstream>();
    auto writer = StreamWriter(stream, boost::endian::order::big);
    auto ss = ostringstream();
    ss << string("\x40", 1);
    ss << string("\x41", 1);
    ss << string("\x00\x01", 2);
    ss << string("\x00\x00\x00\x02", 4);
    ss << string("\xff\xfd", 2);
    ss << string("\xff\xff\xff\xfc", 4);
    ss << string("\xff\xff\xff\xff\xff\xff\xff\xfb", 8);
    ss << string("\x3f\x80\x00\x00", 4);
    auto expectedOutput = ss.str();

    // when
    writer.putByte(0x40);
    writer.putChar('A');
    writer.putUint16(1);
    writer.putUint32(2);
    writer.putInt16(-3);
    writer.putInt32(-4);
    writer.putInt64(-5);
    writer.putFloat(1.0f);

    // then
    auto output = stream->str();
    BOOST_TEST((expectedOutput == output), notEqualMessage(expectedOutput, output));
}

BOOST_AUTO_TEST_SUITE_END()
