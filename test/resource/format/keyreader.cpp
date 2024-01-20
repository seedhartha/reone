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

#include "reone/resource/format/keyreader.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

using namespace reone;
using namespace reone::resource;

TEST(key_reader, should_read_key) {
    // given

    auto input = StringBuilder()
                     // header
                     .append("KEY V1  ")
                     .append("\x02\x00\x00\x00", 4) // number of files
                     .append("\x02\x00\x00\x00", 4) // number of keys
                     .append("\x40\x00\x00\x00", 4) // offset to files
                     .append("\x5e\x00\x00\x00", 4) // offset to keys
                     .append("\x00\x00\x00\x00", 4) // build year
                     .append("\x00\x00\x00\x00", 4) // build day
                     // reserved
                     .append("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32)
                     // file 0
                     .append("\x80\x00\x00\x00", 4) // filesize
                     .append("\x58\x00\x00\x00", 4) // filename offset
                     .append("\x02\x00", 2)         // filename length
                     .append("\x00\x00", 2)         // drives
                     // file 1
                     .append("\x00\x01\x00\x00", 4) // filesize
                     .append("\x5b\x00\x00\x00", 4) // filename offset
                     .append("\x02\x00", 2)         // filename length
                     .append("\x00\x00", 2)         // drives
                     // filenames
                     .append("Aa\x00", 3)
                     .append("Bb\x00", 3)
                     // key 0
                     .append("Cc\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16)
                     .append("\xe1\x07", 2)
                     .append("\xd3\x07\xc0\x00", 4)
                     // key 1
                     .append("Dd\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16)
                     .append("\xf5\x07", 2)
                     .append("\xd3\x07\xc0\x00", 4)
                     .string();

    auto stream = MemoryInputStream(input);
    auto reader = KeyReader(stream);

    // when

    reader.load();

    auto keys = reader.keys();
    auto files = reader.files();

    // then

    EXPECT_EQ(2ll, files.size());
    EXPECT_EQ(128, files[0].fileSize);
    EXPECT_EQ("Aa", files[0].filename);
    EXPECT_EQ(256, files[1].fileSize);
    EXPECT_EQ("Bb", files[1].filename);
    EXPECT_EQ(2ll, keys.size());
    EXPECT_EQ("cc", keys[0].resId.resRef.value());
    EXPECT_EQ(static_cast<int>(ResType::TwoDA), static_cast<int>(keys[0].resId.type));
    EXPECT_EQ(12, keys[0].bifIdx);
    EXPECT_EQ(2003, keys[0].resIdx);
    EXPECT_EQ("dd", keys[1].resId.resRef.value());
    EXPECT_EQ(static_cast<int>(ResType::Gff), static_cast<int>(keys[1].resId.type));
    EXPECT_EQ(12, keys[1].bifIdx);
    EXPECT_EQ(2003, keys[1].resIdx);
}
