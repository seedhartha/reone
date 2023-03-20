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

#include "../../src/common/binaryreader.h"
#include "../../src/common/stream/bytearrayinput.h"
#include "../../src/common/stringbuilder.h"


#include "../checkutil.h"

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_SUITE(binary_reader)

BOOST_AUTO_TEST_CASE(should_seek_ignore_and_tell_in_little_endian_stream) {
    // given
    auto input = ByteArray("Hello, world!\x00", 13);
    auto stream = ByteArrayInputStream(input);
    auto reader = BinaryReader(stream, boost::endian::order::little);
    auto expectedPos = 7ll;

    // when
    reader.seek(5);
    reader.ignore(2);
    auto actualPos = reader.tell();

    // then
    BOOST_TEST(expectedPos == actualPos);
}

BOOST_AUTO_TEST_CASE(should_read_from_little_endian_stream) {
    // given
    auto input = StringBuilder()
                     .append("\xff", 1)
                     .append("\x01\xff", 2)
                     .append("\x02\xff\xff\xff", 4)
                     .append("\x03\xff\xff\xff\xff\xff\xff\xff", 8)
                     .append("\x01\xff", 2)
                     .append("\x02\xff\xff\xff", 4)
                     .append("\x03\xff\xff\xff\xff\xff\xff\xff", 8)
                     .append("\x00\x00\x80\x3f", 4)
                     .append("\x00\x00\x00\x00\x00\x00\xf0\x3f", 8)
                     .append("Hello, world!")
                     .append("Hello, world!\x00", 14)
                     .append("\x01\x02\x03\x04", 4)
                     .build();
    auto inputBytes = ByteArray();
    inputBytes.resize(input.size());
    inputBytes.insert(inputBytes.begin(), input.begin(), input.end());
    auto stream = ByteArrayInputStream(inputBytes);
    auto reader = BinaryReader(stream, boost::endian::order::little);
    auto expectedByte = 255u;
    auto expectedUint16 = 65281u;
    auto expectedUint32 = 4294967042u;
    auto expectedUint64 = 18446744073709551363u;
    auto expectedInt16 = -255;
    auto expectedInt32 = -254;
    auto expectedInt64 = -253;
    auto expectedFloat = 1.0f;
    auto expectedDouble = 1.0;
    auto expectedStr = string("Hello, world!");
    auto expectedCStr = string("Hello, world!");
    auto expectedBytes = ByteArray({0x01, 0x02, 0x03, 0x04});

    // when
    auto actualByte = reader.getByte();
    auto actualUint16 = reader.getUint16();
    auto actualUint32 = reader.getUint32();
    auto actualUint64 = reader.getUint64();
    auto actualInt16 = reader.getInt16();
    auto actualInt32 = reader.getInt32();
    auto actualInt64 = reader.getInt64();
    auto actualFloat = reader.getFloat();
    auto actualDouble = reader.getDouble();
    auto actualStr = reader.getString(13);
    auto actualCStr = reader.getNullTerminatedString();
    auto actualBytes = reader.getBytes(4);

    // then
    BOOST_TEST(expectedByte == actualByte);
    BOOST_TEST(expectedUint16 == actualUint16);
    BOOST_TEST(expectedUint32 == actualUint32);
    BOOST_TEST(expectedUint64 == actualUint64);
    BOOST_TEST(expectedInt16 == actualInt16);
    BOOST_TEST(expectedInt32 == actualInt32);
    BOOST_TEST(expectedInt64 == actualInt64);
    BOOST_TEST(expectedFloat == actualFloat);
    BOOST_TEST(expectedDouble == actualDouble);
    BOOST_TEST((expectedStr == actualStr), notEqualMessage(expectedStr, actualStr));
    BOOST_TEST((expectedCStr == actualCStr), notEqualMessage(expectedCStr, actualCStr));
    BOOST_TEST((expectedBytes == actualBytes), notEqualMessage(expectedBytes, actualBytes));
}

BOOST_AUTO_TEST_CASE(should_read_from_big_endian_stream) {
    // given
    auto input = StringBuilder()
                     .append("\xff\x01", 2)
                     .append("\xff\xff\xff\x02", 4)
                     .append("\xff\xff\xff\xff\xff\xff\xff\x03", 8)
                     .append("\xff\x01", 2)
                     .append("\xff\xff\xff\x02", 4)
                     .append("\xff\xff\xff\xff\xff\xff\xff\x03", 8)
                     .append("\x3f\x80\x00\x00", 4)
                     .append("\x3f\xf0\x00\x00\x00\x00\x00\x00", 8)
                     .build();
    auto inputBytes = ByteArray();
    inputBytes.resize(input.size());
    inputBytes.insert(inputBytes.begin(), input.begin(), input.end());
    auto stream = ByteArrayInputStream(inputBytes);
    auto reader = BinaryReader(stream, boost::endian::order::big);
    auto expectedUint16 = 65281u;
    auto expectedUint32 = 4294967042u;
    auto expectedUint64 = 18446744073709551363u;
    auto expectedInt16 = -255;
    auto expectedInt32 = -254;
    auto expectedInt64 = -253;
    auto expectedFloat = 1.0f;
    auto expectedDouble = 1.0;

    // when
    auto actualUint16 = reader.getUint16();
    auto actualUint32 = reader.getUint32();
    auto actualUint64 = reader.getUint64();
    auto actualInt16 = reader.getInt16();
    auto actualInt32 = reader.getInt32();
    auto actualInt64 = reader.getInt64();
    auto actualFloat = reader.getFloat();
    auto actualDouble = reader.getDouble();

    // then
    BOOST_TEST(expectedUint16 == actualUint16);
    BOOST_TEST(expectedUint32 == actualUint32);
    BOOST_TEST(expectedUint64 == actualUint64);
    BOOST_TEST(expectedInt16 == actualInt16);
    BOOST_TEST(expectedInt32 == actualInt32);
    BOOST_TEST(expectedInt64 == actualInt64);
    BOOST_TEST(expectedFloat == actualFloat);
    BOOST_TEST(expectedDouble == actualDouble);
}

BOOST_AUTO_TEST_SUITE_END()
