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

#include "../../src/common/logutil.h"
#include "../../src/common/stream/fileoutput.h"
#include "../../src/resource/strings.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(strings)

BOOST_AUTO_TEST_CASE(should_init_talktable_and_get_string_and_sound) {
    // given

    setLogLevel(LogLevel::None);

    auto tmpDirPath = fs::temp_directory_path();
    tmpDirPath.append("reone_test_strings");
    fs::create_directory(tmpDirPath);

    auto tlkPath = tmpDirPath;
    tlkPath.append("dialog.tlk");
    auto tlk = FileOutputStream(tlkPath, OpenMode::Binary);
    tlk.write("TLK V3.0");
    tlk.write("\x00\x00\x00\x00", 4);
    tlk.write("\x01\x00\x00\x00", 4);
    tlk.write("\x3c\x00\x00\x00", 4);
    // String 0 Data
    tlk.write("\x03\x00\x00\x00", 4);
    tlk.write("some_sound\x00\x00\x00\x00\x00\x00", 16);
    tlk.write("\x00\x00\x00\x00", 4);
    tlk.write("\x00\x00\x00\x00", 4);
    tlk.write("\x00\x00\x00\x00", 4);
    tlk.write("\x0d\x00\x00\x00", 4);
    tlk.write("\x00\x00\x00\x00", 4);
    // Strings 0 Entry
    tlk.write("Hello, world!");
    //
    tlk.close();

    auto strings = Strings();

    auto expectedText = string("Hello, world!");
    auto expectedSound = string("some_sound");

    // when

    strings.init(tmpDirPath);
    auto text = strings.get(0);
    auto sound = strings.getSound(0);

    // then

    BOOST_TEST(expectedText == text);
    BOOST_TEST(expectedSound == sound);

    // cleanup

    fs::remove_all(tmpDirPath);
}

BOOST_AUTO_TEST_SUITE_END()
