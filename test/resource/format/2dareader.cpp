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

#include "reone/resource/2da.h"
#include "reone/resource/format/2dareader.h"
#include "reone/system/binarywriter.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

using namespace reone;
using namespace reone::resource;

TEST(two_da_reader, should_read_two_da) {
    // given

    auto input = StringBuilder()
                     .append("2DA V2.b")
                     .append("\x0a", 1)
                     .append("key\x09", 4)
                     .append("value\x09", 6)
                     .append("\x00", 1)
                     .append("\x02\x00\x00\x00", 4)
                     .append("\x30\x09\x31\x09", 4)
                     .append("\x00\x00", 2)
                     .append("\x07\x00", 2)
                     .append("\x07\x00", 2)
                     .append("\x07\x00", 2)
                     .append("\x0c\x00", 2)
                     .append("unique\x00", 7)
                     .append("same\x00", 5)
                     .string();

    auto stream = MemoryInputStream(input);
    auto reader = TwoDAReader(stream);

    // when

    reader.load();

    // then

    auto twoDa = reader.twoDA();
    EXPECT_EQ(twoDa->getColumnCount(), 2);
    EXPECT_EQ(twoDa->getRowCount(), 2);
    EXPECT_EQ(std::string("unique"), twoDa->getString(0, "key"));
    EXPECT_EQ(std::string("same"), twoDa->getString(0, "value"));
    EXPECT_EQ(std::string("same"), twoDa->getString(1, "key"));
    EXPECT_EQ(std::string("same"), twoDa->getString(1, "value"));
}
