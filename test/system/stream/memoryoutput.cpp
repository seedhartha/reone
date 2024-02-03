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

#include "reone/system/stream/memoryoutput.h"

#include "../../checkutil.h"

using namespace reone;

TEST(MemoryOutputStream, should_write_to_byte_buffer) {
    // given
    auto bytes = ByteBuffer();
    auto stream = MemoryOutputStream(bytes);
    auto bytesToWrite = ByteBuffer {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
    auto expectedOutput = ByteBuffer {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!', '\n', 'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};

    // when
    stream.write(&bytesToWrite[0], bytesToWrite.size());
    auto position = stream.position();
    stream.writeByte('\n');
    stream.writeByte('H');
    stream.write(&bytesToWrite[1], 12);

    // then
    EXPECT_EQ(13ll, position);
    EXPECT_EQ(expectedOutput, bytes) << notEqualMessage(expectedOutput, bytes);
}
