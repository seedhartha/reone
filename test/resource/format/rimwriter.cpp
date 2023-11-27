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

#include "reone/resource/format/rimwriter.h"
#include "reone/system/stream/memoryoutput.h"
#include "reone/system/stringbuilder.h"

#include "../../checkutil.h"

using namespace reone;
using namespace reone::resource;

TEST(rim_writer, should_write_rim) {
    // given

    auto expectedOutput = StringBuilder()
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

    auto bytes = ByteBuffer();
    auto stream = MemoryOutputStream(bytes);

    auto writer = RimWriter();
    writer.add(RimWriter::Resource {"Aa", ResType::Txi, ByteBuffer {'B', 'b'}});

    // when

    writer.save(stream);

    // then

    auto actualOutput = std::string(&bytes[0], bytes.size());
    EXPECT_EQ(expectedOutput, actualOutput) << notEqualMessage(expectedOutput, actualOutput);
}
