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

#include "reone/resource/format/bifreader.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

using namespace reone;
using namespace reone::resource;

TEST(bif_reader, should_read_bif) {
    // given

    auto input = StringBuilder()
                     // header
                     .append("BIFFV1  ")
                     .append("\x01\x00\x00\x00", 4) // number of variable resources
                     .append("\x00\x00\x00\x00", 4) // number of fixed resources
                     .append("\x14\x00\x00\x00", 4) // offset to variable resources
                     // variable resource table
                     .append("\x00\x00\x00\x00", 4) // id
                     .append("\x24\x00\x00\x00", 4) // offset
                     .append("\x0d\x00\x00\x00", 4) // filesize
                     .append("\xe6\x07\x00\x00", 4) // type
                     // variable resource data
                     .append("Hello, world!")
                     .string();

    auto stream = MemoryInputStream(input);
    auto reader = BifReader(stream);

    // when

    reader.load();

    // then

    auto &resources = reader.resources();
    EXPECT_EQ(1ll, resources.size());
    EXPECT_EQ(0, resources[0].id);
    EXPECT_EQ(36, resources[0].offset);
    EXPECT_EQ(13, resources[0].fileSize);
    EXPECT_EQ(2022, resources[0].resType);
}
