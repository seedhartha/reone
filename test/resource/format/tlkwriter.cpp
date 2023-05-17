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

#include <boost/test/unit_test.hpp>

#include "reone/resource/format/tlkwriter.h"
#include "reone/resource/talktable.h"
#include "reone/system/stream/bytearrayoutput.h"
#include "reone/system/stringbuilder.h"


#include "../../checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(tlk_writer)

BOOST_AUTO_TEST_CASE(should_write_tlk) {
    // given

    auto expectedOutput = StringBuilder()
                              // header
                              .append("TLK V3.0", 8)
                              .append("\x00\x00\x00\x00", 4) // language id
                              .append("\x02\x00\x00\x00", 4) // number of strings
                              .append("\x64\x00\x00\x00", 4) // offset to string entries
                              // string data 0
                              .append("\x07\x00\x00\x00", 4)                                                  // flags
                              .append("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) // sound res ref
                              .append("\x00\x00\x00\x00", 4)                                                  // volume variance
                              .append("\x00\x00\x00\x00", 4)                                                  // pitch variance
                              .append("\x00\x00\x00\x00", 4)                                                  // offset to string
                              .append("\x04\x00\x00\x00", 4)                                                  // string size
                              .append("\x00\x00\x00\x00", 4)                                                  // sound length
                              // string data 1
                              .append("\x07\x00\x00\x00", 4)                                      // flags
                              .append("jane\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) // sound res ref
                              .append("\x00\x00\x00\x00", 4)                                      // volume variance
                              .append("\x00\x00\x00\x00", 4)                                      // pitch variance
                              .append("\x04\x00\x00\x00", 4)                                      // offset to string
                              .append("\x04\x00\x00\x00", 4)                                      // string size
                              .append("\x00\x00\x00\x00", 4)                                      // sound length
                              // string entries
                              .append("John")
                              .append("Jane")
                              .build();

    auto bytes = ByteArray();
    auto stream = ByteArrayOutputStream(bytes);

    auto strings = vector<TalkTable::String> {
        TalkTable::String {"John", ""},
        TalkTable::String {"Jane", "jane"}};

    auto table = TalkTable(std::move(strings));
    auto writer = TlkWriter(table);

    // when

    writer.save(stream);

    // then

    auto actualOutput = string(&bytes[0], bytes.size());
    BOOST_TEST((expectedOutput == actualOutput), notEqualMessage(expectedOutput, actualOutput));
}

BOOST_AUTO_TEST_SUITE_END()
