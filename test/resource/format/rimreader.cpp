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

#include "reone/resource/format/rimreader.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

using namespace reone;
using namespace reone::resource;

TEST(rim_reader, should_read_rim) {
    // given

    auto input = StringBuilder()
                     // header
                     .append("RIM V1.0")
                     .append("\x00\x00\x00\x00", 4) // reserved
                     .append("\x01\x00\x00\x00", 4) // number of resources
                     .append("\x78\x00\x00\x00", 4) // offset to resources
                     .append('\x00', 100)           // reserved
                     // resources
                     .append("Aa\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) // resref
                     .append("\xe6\x07\x00\x00", 4)                                            // type
                     .append("\x00\x00\x00\x00", 4)                                            // id
                     .append("\x98\x00\x00\x00", 4)                                            // offset
                     .append("\x02\x00\x00\x00", 4)                                            // size
                     // resource data
                     .append("Bb", 2)
                     .string();

    auto stream = MemoryInputStream(input);
    auto reader = RimReader(stream);

    // when

    reader.load();

    // then

    auto resources = reader.resources();
    EXPECT_EQ(1ll, resources.size());
    EXPECT_EQ("aa", resources.front().resId.resRef);
    EXPECT_EQ(static_cast<int>(ResourceType::Txi), static_cast<int>(resources.front().resId.type));
    EXPECT_EQ(152, resources.front().offset);
    EXPECT_EQ(2, resources.front().size);
}
