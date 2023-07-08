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

#include <gtest/gtest.h>

#include "reone/system/binaryreader.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

#include "../checkutil.h"

using namespace reone;

TEST(binary_reader, should_seek_ignore_and_tell_in_little_endian_stream) {
    // given
    auto input = ByteArray {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!', '\x00'};
    auto stream = MemoryInputStream(input);
    auto reader = BinaryReader(stream, boost::endian::order::little);
    auto expectedPos = 7ll;

    // when
    reader.seek(5);
    reader.ignore(2);
    auto actualPos = reader.position();

    // then
    EXPECT_EQ(expectedPos, actualPos);
}

TEST(binary_reader, should_read_from_little_endian_stream) {
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
    auto stream = MemoryInputStream(inputBytes);
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
    auto expectedStr = std::string("Hello, world!");
    auto expectedCStr = std::string("Hello, world!");
    auto expectedBytes = ByteArray({0x01, 0x02, 0x03, 0x04});

    // when
    auto actualByte = reader.readByte();
    auto actualUint16 = reader.readUint16();
    auto actualUint32 = reader.readUint32();
    auto actualUint64 = reader.readUint64();
    auto actualInt16 = reader.readInt16();
    auto actualInt32 = reader.readInt32();
    auto actualInt64 = reader.readInt64();
    auto actualFloat = reader.readFloat();
    auto actualDouble = reader.readDouble();
    auto actualStr = reader.readString(13);
    auto actualCStr = reader.readCString();
    auto actualBytes = reader.readBytes(4);

    // then
    EXPECT_EQ(expectedByte, actualByte);
    EXPECT_EQ(expectedUint16, actualUint16);
    EXPECT_EQ(expectedUint32, actualUint32);
    EXPECT_EQ(expectedUint64, actualUint64);
    EXPECT_EQ(expectedInt16, actualInt16);
    EXPECT_EQ(expectedInt32, actualInt32);
    EXPECT_EQ(expectedInt64, actualInt64);
    EXPECT_EQ(expectedFloat, actualFloat);
    EXPECT_EQ(expectedDouble, actualDouble);
    EXPECT_EQ(expectedStr, actualStr) << notEqualMessage(expectedStr, actualStr);
    EXPECT_EQ(expectedCStr, actualCStr) << notEqualMessage(expectedCStr, actualCStr);
    EXPECT_EQ(expectedBytes, actualBytes) << notEqualMessage(expectedBytes, actualBytes);
}

TEST(binary_reader, should_read_from_big_endian_stream) {
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
    auto stream = MemoryInputStream(inputBytes);
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
    auto actualUint16 = reader.readUint16();
    auto actualUint32 = reader.readUint32();
    auto actualUint64 = reader.readUint64();
    auto actualInt16 = reader.readInt16();
    auto actualInt32 = reader.readInt32();
    auto actualInt64 = reader.readInt64();
    auto actualFloat = reader.readFloat();
    auto actualDouble = reader.readDouble();

    // then
    EXPECT_EQ(expectedUint16, actualUint16);
    EXPECT_EQ(expectedUint32, actualUint32);
    EXPECT_EQ(expectedUint64, actualUint64);
    EXPECT_EQ(expectedInt16, actualInt16);
    EXPECT_EQ(expectedInt32, actualInt32);
    EXPECT_EQ(expectedInt64, actualInt64);
    EXPECT_EQ(expectedFloat, actualFloat);
    EXPECT_EQ(expectedDouble, actualDouble);
}
