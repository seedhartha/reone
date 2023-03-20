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
#include "../../../src/graphics/format/txireader.h"

using namespace std;

using namespace reone;
using namespace reone::graphics;

BOOST_AUTO_TEST_SUITE(txi_reader)

BOOST_AUTO_TEST_CASE(should_load_txi) {
    // given
    auto txiBytes = StringBuilder()
                        .append("blending additive")
                        .build();
    auto txi = ByteArrayInputStream(txiBytes);
    auto reader = TxiReader();

    // when
    reader.load(txi);

    // then
    auto features = reader.features();
    BOOST_TEST(static_cast<int>(Texture::Blending::Additive) == static_cast<int>(features.blending));
}

BOOST_AUTO_TEST_SUITE_END()
