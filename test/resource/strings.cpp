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

#include "reone/resource/strings.h"
#include "reone/system/binarywriter.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileoutput.h"

using namespace reone;
using namespace reone::resource;

TEST(strings, should_init_talktable_and_get_string_and_sound) {
    // given

    auto tmpDirPath = boost::filesystem::temp_directory_path();
    tmpDirPath.append("reone_test_strings");
    boost::filesystem::create_directory(tmpDirPath);

    auto tlkPath = tmpDirPath;
    tlkPath.append("dialog.tlk");
    auto tlk = FileOutputStream(tlkPath);
    tlk.write("TLK V3.0", 8);
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
    tlk.write("Hello, world!", 14);
    //
    tlk.close();

    auto strings = Strings();

    auto expectedText = std::string("Hello, world!");
    auto expectedSound = std::string("some_sound");

    // when

    strings.init(tmpDirPath);
    auto text = strings.get(0);
    auto sound = strings.getSound(0);

    // then

    EXPECT_EQ(expectedText, text);
    EXPECT_EQ(expectedSound, sound);

    // cleanup

    boost::filesystem::remove_all(tmpDirPath);
}
