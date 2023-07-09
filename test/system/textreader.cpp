
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

using namespace reone;

TEST(text_reader, should_read_lines_from_byte_buffer) {
    // given
    auto bytes = ByteBuffer {'l', 'i', 'n', 'e', '1', '\r', '\n', 'l', 'i', 'n', 'e', '2', '\n', 'l', 'o', 'n', 'g', 'l', 'i', 'n', 'e'};
    auto stream = MemoryInputStream(bytes);
    auto reader = TextReader(stream);
    std::string line;
    bool read;

    // expect
    read = reader.readLine(line);
    EXPECT_EQ(line, std::string("line1"));
    EXPECT_TRUE(read);

    read = reader.readLine(line);
    EXPECT_EQ(line, std::string("line2"));
    EXPECT_TRUE(read);

    read = reader.readLine(line);
    EXPECT_EQ(line, std::string("longline"));
    EXPECT_TRUE(read);

    read = reader.readLine(line);
    EXPECT_EQ(line, std::string());
    EXPECT_TRUE(!read);
}