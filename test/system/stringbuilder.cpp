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

#include "reone/system/stringbuilder.h"

#include "../checkutil.h"

using namespace reone;

TEST(StringBuilder, should_build_a_string) {
    // given
    auto expectedStr = std::string("Hello, world!");

    // when
    auto str = StringBuilder()
                   .append("He")
                   .append('l', 2)
                   .append("o, world!")
                   .string();

    // then
    EXPECT_EQ(expectedStr, str) << notEqualMessage(expectedStr, str);
}
