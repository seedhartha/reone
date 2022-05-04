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

#include "../../src/common/stream/bytearrayoutput.h"
#include "../../src/common/streamwriter.h"
#include "../../src/common/stringbuilder.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_SUITE(stream_writer)

BOOST_AUTO_TEST_CASE(should_write_to_little_endian_stream) {
    // given
    auto bytes = ByteArray();
    auto stream = ByteArrayOutputStream(bytes);
    auto writer = StreamWriter(stream, boost::endian::order::little);
    auto expectedOutput = StringBuilder()
                              .append("\x40", 1)
                              .append("\x41", 1)
                              .append("\x01\x00", 2)
                              .append("\x02\x00\x00\x00", 4)
                              .append("\xfd\xff", 2)
                              .append("\xfc\xff\xff\xff", 4)
                              .append("\xfb\xff\xff\xff\xff\xff\xff\xff", 8)
                              .append("\x00\x00\x80\x3f", 4)
                              .append("AaBbCc\x00", 7)
                              .append("\x01\x02\x03\x04", 4)
                              .build();

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
    auto output = string(bytes.data(), bytes.size());
    BOOST_TEST((expectedOutput == output), notEqualMessage(expectedOutput, output));
}

BOOST_AUTO_TEST_CASE(should_write_to_big_endian_stream) {
    // given
    auto bytes = ByteArray();
    auto stream = ByteArrayOutputStream(bytes);
    auto writer = StreamWriter(stream, boost::endian::order::big);
    auto expectedOutput = StringBuilder()
                              .append("\x40", 1)
                              .append("\x41", 1)
                              .append("\x00\x01", 2)
                              .append("\x00\x00\x00\x02", 4)
                              .append("\xff\xfd", 2)
                              .append("\xff\xff\xff\xfc", 4)
                              .append("\xff\xff\xff\xff\xff\xff\xff\xfb", 8)
                              .append("\x3f\x80\x00\x00", 4)
                              .build();

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
    auto output = string(bytes.data(), bytes.size());
    BOOST_TEST((expectedOutput == output), notEqualMessage(expectedOutput, output));
}

BOOST_AUTO_TEST_SUITE_END()
