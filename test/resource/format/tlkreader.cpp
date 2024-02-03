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

#include "reone/resource/format/tlkreader.h"
#include "reone/resource/talktable.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

using namespace reone;
using namespace reone::resource;

TEST(TlkReader, should_read_tlk) {
    // given

    auto input = StringBuilder()
                     // header
                     .append("TLK V3.0", 8)
                     .append("\x00\x00\x00\x00", 4) // language id
                     .append("\x02\x00\x00\x00", 4) // number of strings
                     .append("\x64\x00\x00\x00", 4) // offset to std::string entries
                     // std::string data 0
                     .append("\x07\x00\x00\x00", 4)                                                  // flags
                     .append("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) // sound res ref
                     .append("\x00\x00\x00\x00", 4)                                                  // volume variance
                     .append("\x00\x00\x00\x00", 4)                                                  // pitch variance
                     .append("\x00\x00\x00\x00", 4)                                                  // offset to string
                     .append("\x04\x00\x00\x00", 4)                                                  // std::string size
                     .append("\x00\x00\x00\x00", 4)                                                  // sound length
                     // std::string data 1
                     .append("\x07\x00\x00\x00", 4)                                      // flags
                     .append("jane\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) // sound res ref
                     .append("\x00\x00\x00\x00", 4)                                      // volume variance
                     .append("\x00\x00\x00\x00", 4)                                      // pitch variance
                     .append("\x04\x00\x00\x00", 4)                                      // offset to string
                     .append("\x04\x00\x00\x00", 4)                                      // std::string size
                     .append("\x00\x00\x00\x00", 4)                                      // sound length
                     // std::string entries
                     .append("John")
                     .append("Jane")
                     .string();

    auto stream = MemoryInputStream(input);
    auto reader = TlkReader(stream);

    // when

    reader.load();

    // then

    auto table = reader.table();
    EXPECT_EQ(2, table->getStringCount());
    EXPECT_EQ("John", table->getString(0).text);
    EXPECT_EQ("", table->getString(0).soundResRef);
    EXPECT_EQ("Jane", table->getString(1).text);
    EXPECT_EQ("jane", table->getString(1).soundResRef);
}
