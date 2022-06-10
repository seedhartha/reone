/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../../src/common/stream/fileinput.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(file_input_stream)

BOOST_AUTO_TEST_CASE(should_read_from_file) {
    // given

    auto tmpPath = fs::temp_directory_path();
    tmpPath.append("reone_test_file_input");
    auto tmpFile = fs::ofstream(tmpPath);
    tmpFile.write("Hello, world!", 13);
    tmpFile.close();

    auto stream = FileInputStream(tmpPath);
    auto buf = ByteArray(16, '\0');
    auto expectedContents = string("Hello, world!");

    // when

    stream.seek(0, SeekOrigin::End);
    size_t position1 = stream.position();
    stream.seek(0, SeekOrigin::Begin);
    int readByteResult1 = stream.readByte();
    stream.seek(-1, SeekOrigin::Current);
    size_t position2 = stream.position();
    int readResult = stream.read(&buf[0], 16);
    auto contents = buf.substr(0, 13);
    int readByteResult2 = stream.readByte();
    bool eof = stream.eof();
    stream.close();

    // then

    BOOST_CHECK_EQUAL(13ll, position1);
    BOOST_CHECK_EQUAL('H', readByteResult1);
    BOOST_CHECK_EQUAL(0ll, position2);
    BOOST_CHECK_EQUAL(13, readResult);
    BOOST_CHECK_EQUAL(16ll, buf.size());
    BOOST_TEST((expectedContents == contents), notEqualMessage(expectedContents, contents));
    BOOST_CHECK_EQUAL(-1, readByteResult2);
    BOOST_CHECK_EQUAL(true, eof);

    // cleanup

    fs::remove(tmpPath);
}

BOOST_AUTO_TEST_SUITE_END()
