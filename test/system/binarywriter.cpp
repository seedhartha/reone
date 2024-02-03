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

#include "reone/system/binarywriter.h"
#include "reone/system/stream/memoryoutput.h"
#include "reone/system/stringbuilder.h"

#include "../checkutil.h"

using namespace reone;

TEST(BinaryWriter, should_write_to_little_endian_stream) {
    // given
    auto bytes = ByteBuffer();
    auto stream = MemoryOutputStream(bytes);
    auto writer = BinaryWriter(stream, boost::endian::order::little);
    auto expectedOutput = StringBuilder()
                              .append("\x40", 1)
                              .append("\x41", 1)
                              .append("\x01\x00", 2)
                              .append("\x02\x00\x00\x00", 4)
                              .append("\xfd\xff", 2)
                              .append("\xfc\xff\xff\xff", 4)
                              .append("\xfb\xff\xff\xff\xff\xff\xff\xff", 8)
                              .append("\x00\x00\x80\x3f", 4)
                              .append("AaBb\x00", 5)
                              .append("\x01\x02\x03\x04", 4)
                              .string();

    // when
    writer.writeByte(0x40);
    writer.writeChar('A');
    writer.writeUint16(1);
    writer.writeUint32(2);
    writer.writeInt16(-3);
    writer.writeInt32(-4);
    writer.writeInt64(-5);
    writer.writeFloat(1.0f);
    writer.writeString(std::string("Aa"));
    writer.writeCString(std::string("Bb\x00"));
    writer.write(ByteBuffer {0x01, 0x02, 0x03, 0x04});

    // then
    auto output = std::string(bytes.data(), bytes.size());
    EXPECT_EQ(expectedOutput, output) << notEqualMessage(expectedOutput, output);
}

TEST(BinaryWriter, should_write_to_big_endian_stream) {
    // given
    auto bytes = ByteBuffer();
    auto stream = MemoryOutputStream(bytes);
    auto writer = BinaryWriter(stream, boost::endian::order::big);
    auto expectedOutput = StringBuilder()
                              .append("\x40", 1)
                              .append("\x41", 1)
                              .append("\x00\x01", 2)
                              .append("\x00\x00\x00\x02", 4)
                              .append("\xff\xfd", 2)
                              .append("\xff\xff\xff\xfc", 4)
                              .append("\xff\xff\xff\xff\xff\xff\xff\xfb", 8)
                              .append("\x3f\x80\x00\x00", 4)
                              .string();

    // when
    writer.writeByte(0x40);
    writer.writeChar('A');
    writer.writeUint16(1);
    writer.writeUint32(2);
    writer.writeInt16(-3);
    writer.writeInt32(-4);
    writer.writeInt64(-5);
    writer.writeFloat(1.0f);

    // then
    auto output = std::string(bytes.data(), bytes.size());
    EXPECT_EQ(expectedOutput, output) << notEqualMessage(expectedOutput, output);
}
