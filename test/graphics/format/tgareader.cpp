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

#include "../../../src/common/stream/bytearrayinput.h"
#include "../../../src/common/stringbuilder.h"
#include "../../../src/graphics/format/tgareader.h"
#include "../../../src/graphics/texture.h"

using namespace std;

using namespace reone;
using namespace reone::graphics;

BOOST_AUTO_TEST_SUITE(tga_reader)

BOOST_AUTO_TEST_CASE(should_load_tga) {
    // given
    auto tgaBytes = StringBuilder()
                        // Header
                        .append("\x00", 1)     // id length
                        .append("\x00", 1)     // unknown
                        .append("\x03", 1)     // data type
                        .repeat('\x00', 9)     // unknown
                        .append("\x01\x00", 2) // width
                        .append("\x01\x00", 2) // height
                        .append("\x08", 1)     // bits per pixel
                        .append("\x00", 1)     // descriptor
                        // Pixels
                        .append("\xff", 1)
                        .build();
    auto tga = ByteArrayInputStream(tgaBytes);
    auto reader = TgaReader("some_texture", TextureUsage::Default);

    // when
    reader.load(tga);

    // then
    auto texture = reader.texture();
    BOOST_TEST(static_cast<bool>(texture));
    BOOST_TEST(string("some_texture") == texture->name());
    BOOST_TEST(1 == texture->width());
    BOOST_TEST(1 == texture->height());
    BOOST_TEST(1ll == texture->layers().size());
    BOOST_TEST(static_cast<bool>(texture->layers()[0].pixels));
    auto pixels = reinterpret_cast<unsigned char *>(texture->layers()[0].pixels->data());
    BOOST_TEST(255 == pixels[0]);
}

BOOST_AUTO_TEST_SUITE_END()
