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

#include "reone/system/hexutil.h"

#include "../checkutil.h"

using namespace reone;

TEST(hex_util, should_hexify_utf8_string) {
    // given
    auto input = std::string("Hello, world!");
    auto expectedOutput = std::string("48 65 6c 6c 6f 2c 20 77 6f 72 6c 64 21 ");

    // when
    auto output = hexify(input, " ");

    // then
    EXPECT_EQ(expectedOutput, output);
}

TEST(hex_util, should_hexify_byte_buffer) {
    // given
    auto input = ByteBuffer {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
    auto expectedOutput = std::string("48 65 6c 6c 6f 2c 20 77 6f 72 6c 64 21 ");

    // when
    auto output = hexify(input, " ");

    // then
    EXPECT_EQ(expectedOutput, output);
}

TEST(hex_util, should_unhexify_utf8_string) {
    // given
    auto input = std::string("48656c6c6f2c20776f726c6421");
    auto expectedOutput = ByteBuffer {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};

    // when
    auto output = unhexify(input);

    // then
    EXPECT_EQ(expectedOutput, output);
}
