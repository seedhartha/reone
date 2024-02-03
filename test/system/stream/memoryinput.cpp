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

#include "reone/system/stream/memoryinput.h"
#include "reone/system/textreader.h"

#include "../../checkutil.h"

using namespace reone;

TEST(MemoryInputStream, should_read_from_byte_buffer) {
    // given
    auto bytes = ByteBuffer {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
    auto stream = MemoryInputStream(bytes);
    auto buf = ByteBuffer(16, '\0');
    auto expectedContents = std::string("Hello, world!");

    // when
    stream.seek(0, SeekOrigin::End);
    size_t position1 = stream.position();
    stream.seek(0, SeekOrigin::Begin);
    int readByteResult1 = stream.readByte();
    stream.seek(-1, SeekOrigin::Current);
    size_t position2 = stream.position();
    int readResult = stream.read(&buf[0], 16);
    std::string contents;
    contents.insert(contents.begin(), buf.begin(), buf.begin() + 13);
    int readByteResult2 = stream.readByte();

    // then
    EXPECT_EQ(13ll, position1);
    EXPECT_EQ('H', readByteResult1);
    EXPECT_EQ(0ll, position2);
    EXPECT_EQ(13, readResult);
    EXPECT_EQ(16ll, buf.size());
    EXPECT_EQ(expectedContents, contents) << notEqualMessage(expectedContents, contents);
    EXPECT_EQ(-1, readByteResult2);
}
