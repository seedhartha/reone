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

#include "reone/resource/format/erfreader.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

using namespace reone;
using namespace reone::resource;

TEST(erf_reader, should_read_erf) {
    // given

    auto input = StringBuilder()
                     // header
                     .append("ERF V1.0")
                     .append("\x00\x00\x00\x00", 4) // number of languages
                     .append("\x00\x00\x00\x00", 4) // size of localized strings
                     .append("\x01\x00\x00\x00", 4) // number of entries
                     .append("\xa0\x00\x00\x00", 4) // offset to localized strings
                     .append("\xa0\x00\x00\x00", 4) // offset to key list
                     .append("\xb8\x00\x00\x00", 4) // offset to resource list
                     .append("\x00\x00\x00\x00", 4) // build year
                     .append("\x00\x00\x00\x00", 4) // build day
                     .append("\xff\xff\xff\xff", 4) // description strref
                     .append('\x00', 116)           // reserved
                     // key list
                     .append("Aa\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) // resref
                     .append("\x00\x00\x00\x00", 4)                                            // resid
                     .append("\xe6\x07", 2)                                                    // restype
                     .append("\x00\x00", 2)                                                    // unused
                     // resource list
                     .append("\xc0\x00\x00\x00", 4) // offset to resource
                     .append("\x02\x00\x00\x00", 4) // resource size
                     // resource data
                     .append("Bb")
                     .string();

    auto stream = MemoryInputStream(input);
    auto reader = ErfReader(stream);

    // when

    reader.load();

    // then

    EXPECT_EQ(1, reader.keys().size());
    EXPECT_EQ(1, reader.resources().size());
    auto &key = reader.keys().front();
    EXPECT_EQ("aa", key.resId.resRef.value());
    EXPECT_EQ(static_cast<int>(ResType::Txi), static_cast<int>(key.resId.type));
    auto &resource = reader.resources().front();
    EXPECT_EQ(192, resource.offset);
    EXPECT_EQ(2, resource.size);
}
