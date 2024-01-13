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

#include "reone/system/unicodeutil.h"

using namespace reone;

TEST(code_points_from_utf8, should_convert_utf8_string_to_code_points) {
    // given
    auto utf8 = ",.09AZaz\xc2\xa0\xdf\xbf\xe0\xa2\xa0\xef\xbf\xae\xf0\x92\x80\x80\xf0\x9f\xa7\xbf";

    // when
    auto codePoints = codePointsFromUTF8(utf8);

    // then
    EXPECT_EQ(codePoints.size(), 14);
    EXPECT_EQ(codePoints[0], ',');
    EXPECT_EQ(codePoints[1], '.');
    EXPECT_EQ(codePoints[2], '0');
    EXPECT_EQ(codePoints[3], '9');
    EXPECT_EQ(codePoints[4], 'A');
    EXPECT_EQ(codePoints[5], 'Z');
    EXPECT_EQ(codePoints[6], 'a');
    EXPECT_EQ(codePoints[7], 'z');
    EXPECT_EQ(codePoints[8], 0xa0);
    EXPECT_EQ(codePoints[9], 0x7ff);
    EXPECT_EQ(codePoints[10], 0x8a0);
    EXPECT_EQ(codePoints[11], 0xffee);
    EXPECT_EQ(codePoints[12], 0x12000);
    EXPECT_EQ(codePoints[13], 0x1f9ff);
}

TEST(utf8_from_code_points, should_convert_code_points_to_utf8_string) {
    // given
    std::vector<uint32_t> codePoints {
        ',', '.',
        '0', '9',
        'A', 'Z',
        'a', 'z',
        0xa0, 0x7ff,
        0x8a0, 0xffee,
        0x12000, 0x1f9ff};

    // when
    auto utf8 = utf8FromCodePoints(codePoints);

    // then
    EXPECT_EQ(utf8, ",.09AZaz\xc2\xa0\xdf\xbf\xe0\xa2\xa0\xef\xbf\xae\xf0\x92\x80\x80\xf0\x9f\xa7\xbf");
}

TEST(code_points_from_utf16, should_convert_utf16_string_to_code_points) {
    // given
    auto utf16 = u",.09AZaz\u00a0\u07ff\u08a0\uffee\U00012000\U0001f9ff";

    // when
    auto codePoints = codePointsFromUTF16(utf16);

    // then
    EXPECT_EQ(codePoints.size(), 14);
    EXPECT_EQ(codePoints[0], ',');
    EXPECT_EQ(codePoints[1], '.');
    EXPECT_EQ(codePoints[2], '0');
    EXPECT_EQ(codePoints[3], '9');
    EXPECT_EQ(codePoints[4], 'A');
    EXPECT_EQ(codePoints[5], 'Z');
    EXPECT_EQ(codePoints[6], 'a');
    EXPECT_EQ(codePoints[7], 'z');
    EXPECT_EQ(codePoints[8], 0xa0);
    EXPECT_EQ(codePoints[9], 0x7ff);
    EXPECT_EQ(codePoints[10], 0x8a0);
    EXPECT_EQ(codePoints[11], 0xffee);
    EXPECT_EQ(codePoints[12], 0x12000);
    EXPECT_EQ(codePoints[13], 0x1f9ff);
}

TEST(utf16_from_code_points, should_convert_code_points_to_ut16_string) {
    // given
    std::vector<uint32_t> codePoints {
        ',', '.',
        '0', '9',
        'A', 'Z',
        'a', 'z',
        0xa0, 0x7ff,
        0x8a0, 0xffee,
        0x12000, 0x1f9ff};

    // when
    auto utf16 = utf16FromCodePoints(codePoints);

    // then
    EXPECT_EQ(utf16, u",.09AZaz\u00a0\u07ff\u08a0\uffee\U00012000\U0001f9ff");
}

TEST(is_code_point_alpha, should_return_true_for_alpha_chars_in_latin_and_cyrillic) {
    EXPECT_FALSE(isCodePointAlpha(','));
    EXPECT_FALSE(isCodePointAlpha('.'));
    EXPECT_FALSE(isCodePointAlpha('0'));
    EXPECT_FALSE(isCodePointAlpha('9'));
    EXPECT_TRUE(isCodePointAlpha('A'));
    EXPECT_TRUE(isCodePointAlpha('Z'));
    EXPECT_TRUE(isCodePointAlpha('a'));
    EXPECT_TRUE(isCodePointAlpha('z'));
    EXPECT_TRUE(isCodePointAlpha(0x410));
    EXPECT_TRUE(isCodePointAlpha(0x429));
    EXPECT_TRUE(isCodePointAlpha(0x430));
    EXPECT_TRUE(isCodePointAlpha(0x449));
    EXPECT_TRUE(isCodePointAlpha(0x401));
    EXPECT_TRUE(isCodePointAlpha(0x451));
}

TEST(code_point_to_lower, should_convert_alpha_chars_in_latin_in_cyrillic_to_lower_case) {
    EXPECT_EQ(codePointToLower(','), ',');
    EXPECT_EQ(codePointToLower('.'), '.');
    EXPECT_EQ(codePointToLower('0'), '0');
    EXPECT_EQ(codePointToLower('9'), '9');
    EXPECT_EQ(codePointToLower('A'), 'a');
    EXPECT_EQ(codePointToLower('Z'), 'z');
    EXPECT_EQ(codePointToLower('a'), 'a');
    EXPECT_EQ(codePointToLower('z'), 'z');
    EXPECT_EQ(codePointToLower(0x410), 0x430);
    EXPECT_EQ(codePointToLower(0x42f), 0x44f);
    EXPECT_EQ(codePointToLower(0x430), 0x430);
    EXPECT_EQ(codePointToLower(0x44f), 0x44f);
    EXPECT_EQ(codePointToLower(0x401), 0x451);
    EXPECT_EQ(codePointToLower(0x451), 0x451);
}

TEST(code_point_to_upper, should_convert_alpha_chars_in_latin_in_cyrillic_to_upper_case) {
    EXPECT_EQ(codePointToUpper(','), ',');
    EXPECT_EQ(codePointToUpper('.'), '.');
    EXPECT_EQ(codePointToUpper('0'), '0');
    EXPECT_EQ(codePointToUpper('9'), '9');
    EXPECT_EQ(codePointToUpper('A'), 'A');
    EXPECT_EQ(codePointToUpper('Z'), 'Z');
    EXPECT_EQ(codePointToUpper('a'), 'A');
    EXPECT_EQ(codePointToUpper('z'), 'Z');
    EXPECT_EQ(codePointToUpper(0x410), 0x410);
    EXPECT_EQ(codePointToUpper(0x42f), 0x42f);
    EXPECT_EQ(codePointToUpper(0x430), 0x410);
    EXPECT_EQ(codePointToUpper(0x44f), 0x42f);
    EXPECT_EQ(codePointToUpper(0x401), 0x401);
    EXPECT_EQ(codePointToUpper(0x451), 0x401);
}
