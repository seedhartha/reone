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

#include "reone/resource/resources.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileoutput.h"

#include "../checkutil.h"

using namespace reone;
using namespace reone::resource;

TEST(resources, should_index_providers_and_get_resources_without_caching) {
    // given

    auto tmpDirPath = std::filesystem::temp_directory_path();
    tmpDirPath.append("reone_test_resources");
    std::filesystem::create_directory(tmpDirPath);

    auto keyPath = tmpDirPath;
    keyPath.append("sample.key");
    auto key = FileOutputStream(keyPath);
    key.write("KEY V1  ", 8);
    key.write("\x00\x00\x00\x00", 4);
    key.write("\x00\x00\x00\x00", 4);
    key.write("\x00\x00\x00\x00", 4);
    key.write("\x00\x00\x00\x00", 4);
    key.write("\x00\x00\x00\x00", 4);
    key.write("\x00\x00\x00\x00", 4);
    auto keyPadding = ByteBuffer(32, '\0');
    key.write(&keyPadding[0], keyPadding.size());
    key.close();

    auto erfPath = tmpDirPath;
    erfPath.append("sample.erf");
    auto erf = FileOutputStream(erfPath);
    erf.write("ERF V1.0", 8);
    erf.write("\x00\x00\x00\x00", 4);
    erf.write("\x00\x00\x00\x00", 4);
    erf.write("\x00\x00\x00\x00", 4);
    erf.write("\x00\x00\x00\x00", 4);
    erf.write("\x00\x00\x00\x00", 4);
    erf.write("\x00\x00\x00\x00", 4);
    erf.write("\x00\x00\x00\x00", 4);
    erf.write("\x00\x00\x00\x00", 4);
    erf.write("\x00\x00\x00\x00", 4);
    auto erfPadding = ByteBuffer(116, '\0');
    erf.write(&erfPadding[0], erfPadding.size());
    erf.close();

    auto rimPath = tmpDirPath;
    rimPath.append("sample.rim");
    auto rim = FileOutputStream(rimPath);
    rim.write("RIM V1.0", 8);
    rim.write("\x00\x00\x00\x00", 4);
    rim.write("\x00\x00\x00\x00", 4);
    rim.write("\x00\x00\x00\x00", 4);
    rim.close();

    auto overridePath = tmpDirPath;
    overridePath.append("override");
    std::filesystem::create_directory(overridePath);

    auto resPath = overridePath;
    resPath.append("sample.txt");
    auto res = FileOutputStream(resPath);
    res.write("Hello, world!", 13);
    res.close();

    auto resources = Resources();

    auto expectedResData = ByteBuffer {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};

    // when

    resources.indexKEY(keyPath);
    resources.indexERF(erfPath);
    resources.indexFolder(overridePath);
    resources.indexRIM(rimPath);

    auto numProviders = resources.providers().size();
    auto actualResData1 = resources.find(ResourceId("sample", ResourceType::Txt));
    resources.clearProviders();
    auto actualResData2 = resources.find(ResourceId("sample", ResourceType::Txt));

    // then

    EXPECT_EQ(4ll, numProviders);
    EXPECT_TRUE(static_cast<bool>(actualResData1));
    EXPECT_EQ(expectedResData, (*actualResData1)) << notEqualMessage(expectedResData, *actualResData1);
    EXPECT_TRUE(!static_cast<bool>(actualResData2));

    // cleanup

    std::filesystem::remove_all(tmpDirPath);
}
