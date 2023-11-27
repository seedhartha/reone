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

#include "reone/resource/resref.h"

using namespace reone;
using namespace reone::resource;

TEST(res_ref, should_truncate_value_to_16_chars_and_convert_to_lower_case) {
    EXPECT_EQ(ResRef("longANDmixedCASEstring").value(), "longandmixedcase");
}

TEST(res_ref, should_be_equatable) {
    // given
    ResRef resRef1("A");
    ResRef resRef2("a");
    ResRef resRef3("b");

    // expect
    EXPECT_EQ(resRef1, resRef2);
    EXPECT_NE(resRef1, resRef3);
    EXPECT_NE(resRef2, resRef3);
}

TEST(res_ref, should_be_comparable) {
    // given
    ResRef resRef1("a");
    ResRef resRef2("ab");
    ResRef resRef3("baa");

    // expect
    EXPECT_LT(resRef1, resRef2);
    EXPECT_LT(resRef2, resRef3);
    EXPECT_GT(resRef3, resRef1);
    EXPECT_GT(resRef3, resRef2);
}
