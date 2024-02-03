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

#include "reone/graphics/format/txireader.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

using namespace reone;
using namespace reone::graphics;

TEST(TxiReader, should_load_txi) {
    // given
    auto txiBytes = StringBuilder()
                        .append("blending additive")
                        .string();
    auto txi = MemoryInputStream(txiBytes);
    auto reader = TxiReader();

    // when
    reader.load(txi);

    // then
    auto features = reader.features();
    EXPECT_EQ(static_cast<int>(Texture::Blending::Additive), static_cast<int>(features.blending));
}
