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

#include "reone/graphics/lipanimation.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stream/memoryoutput.h"
#include "reone/system/textwriter.h"
#include "reone/tools/lipcomposer.h"

using namespace reone;

TEST(pronouncing_dictionary, should_load_phonemes_from_stream) {
    // given
    auto bytes = ByteBuffer();
    auto ostream = MemoryOutputStream(bytes);
    auto writer = TextWriter(ostream);
    writer.writeLine("hello HH AH0 L OW1");
    writer.writeLine("hello(2) HH EH0 L OW1");
    writer.writeLine("world W ER1 L D");
    auto istream = MemoryInputStream(bytes);
    auto dict = PronouncingDictionary();

    // when
    dict.load(istream);

    // then
    auto phonemes = dict.phonemes("hello");
    EXPECT_EQ(phonemes, (std::vector<std::string> {"HH", "AH0", "L", "OW1"}));
}

TEST(lip_composer, should_compose_lip_file_from_text_with_implicit_word_groups_and_empty_silent_spans) {
    // given
    auto wordToPhonemes = PronouncingDictionary::WordPhonemesMap {
        {"hello", {"HH", "AH0", "L", "OW1"}}, //
        {"world", {"W", "ER1", "L", "D"}}     //
    };
    auto dict = PronouncingDictionary(wordToPhonemes);
    auto composer = LipComposer(dict);

    // when
    auto anim = composer.compose("name", "Hello, world!", 1.0f);

    // then
    EXPECT_TRUE(anim);
    EXPECT_EQ(anim->name(), "name");
    EXPECT_EQ(anim->length(), 1.0f);
    auto &keyframes = anim->keyframes();
    EXPECT_EQ(keyframes.size(), 9);
    EXPECT_FLOAT_EQ(keyframes[0].time, 0.0f);
    EXPECT_FLOAT_EQ(keyframes[1].time, 0.125f);
    EXPECT_FLOAT_EQ(keyframes[2].time, 0.25f);
    EXPECT_FLOAT_EQ(keyframes[3].time, 0.375f);
    EXPECT_FLOAT_EQ(keyframes[4].time, 0.5f);
    EXPECT_FLOAT_EQ(keyframes[5].time, 0.625f);
    EXPECT_FLOAT_EQ(keyframes[6].time, 0.75f);
    EXPECT_FLOAT_EQ(keyframes[7].time, 0.875f);
    EXPECT_FLOAT_EQ(keyframes[8].time, 1.0f);
    EXPECT_EQ(keyframes[0].shape, 9);
    EXPECT_EQ(keyframes[1].shape, 3);
    EXPECT_EQ(keyframes[2].shape, 15);
    EXPECT_EQ(keyframes[3].shape, 4);
    EXPECT_EQ(keyframes[4].shape, 5);
    EXPECT_EQ(keyframes[5].shape, 2);
    EXPECT_EQ(keyframes[6].shape, 15);
    EXPECT_EQ(keyframes[7].shape, 10);
    EXPECT_EQ(keyframes[8].shape, 0);
}

TEST(lip_composer, should_compose_lip_file_from_text_with_explicit_word_groups_and_silent_spans) {
    // given
    auto wordToPhonemes = PronouncingDictionary::WordPhonemesMap {
        {"hello", {"HH", "AH0", "L", "OW1"}},   //
        {"world", {"W", "ER1", "L", "D"}},      //
        {"lorem", {"L", "AO", "R", "EH", "M"}}, //
        {"ipsum", {"IH", "P", "S", "AH0", "M"}} //
    };
    auto dict = PronouncingDictionary(wordToPhonemes);
    auto composer = LipComposer(dict);
    auto silentSpans = std::vector<TimeSpan> {
        {0.0f, 0.1f},   //
        {0.45f, 0.55f}, //
        {0.9, 1.0f}     //
    };

    // when
    auto anim = composer.compose("name", "(Hello, world!) (Lorem ipsum)", 1.0f, silentSpans);

    // then
    EXPECT_TRUE(anim);
    EXPECT_EQ(anim->name(), "name");
    EXPECT_EQ(anim->length(), 1.0f);
    auto &keyframes = anim->keyframes();
    EXPECT_EQ(keyframes.size(), 24);
    EXPECT_FLOAT_EQ(keyframes[0].time, 0.0f);
    EXPECT_NEAR(keyframes[1].time, 0.1f, 0.001f);
    EXPECT_NEAR(keyframes[2].time, 0.1f, 0.001f);
    EXPECT_NEAR(keyframes[3].time, 0.1437f, 0.001f);
    EXPECT_NEAR(keyframes[4].time, 0.1875f, 0.001f);
    EXPECT_NEAR(keyframes[5].time, 0.2312f, 0.001f);
    EXPECT_NEAR(keyframes[6].time, 0.275f, 0.001f);
    EXPECT_NEAR(keyframes[7].time, 0.3187f, 0.001f);
    EXPECT_NEAR(keyframes[8].time, 0.3624f, 0.001f);
    EXPECT_NEAR(keyframes[9].time, 0.4062f, 0.001f);
    EXPECT_NEAR(keyframes[10].time, 0.4499f, 0.001f);
    EXPECT_NEAR(keyframes[11].time, 0.55f, 0.001f);
    EXPECT_NEAR(keyframes[12].time, 0.55f, 0.001f);
    EXPECT_NEAR(keyframes[13].time, 0.585f, 0.001f);
    EXPECT_NEAR(keyframes[14].time, 0.62f, 0.001f);
    EXPECT_NEAR(keyframes[15].time, 0.6549f, 0.001f);
    EXPECT_NEAR(keyframes[16].time, 0.6899f, 0.001f);
    EXPECT_NEAR(keyframes[17].time, 0.7249f, 0.001f);
    EXPECT_NEAR(keyframes[18].time, 0.7599f, 0.001f);
    EXPECT_NEAR(keyframes[19].time, 0.7949f, 0.001f);
    EXPECT_NEAR(keyframes[20].time, 0.8299f, 0.001f);
    EXPECT_NEAR(keyframes[21].time, 0.86499f, 0.001f);
    EXPECT_NEAR(keyframes[22].time, 0.8999f, 0.001f);
    EXPECT_NEAR(keyframes[23].time, 1.0f, 0.001f);
    EXPECT_EQ(keyframes[0].shape, 0);
    EXPECT_EQ(keyframes[1].shape, 0);
    EXPECT_EQ(keyframes[2].shape, 9);
    EXPECT_EQ(keyframes[3].shape, 3);
    EXPECT_EQ(keyframes[4].shape, 15);
    EXPECT_EQ(keyframes[5].shape, 4);
    EXPECT_EQ(keyframes[6].shape, 5);
    EXPECT_EQ(keyframes[7].shape, 2);
    EXPECT_EQ(keyframes[8].shape, 15);
    EXPECT_EQ(keyframes[9].shape, 10);
    EXPECT_EQ(keyframes[10].shape, 0);
    EXPECT_EQ(keyframes[11].shape, 0);
    EXPECT_EQ(keyframes[12].shape, 15);
    EXPECT_EQ(keyframes[13].shape, 4);
    EXPECT_EQ(keyframes[14].shape, 4);
    EXPECT_EQ(keyframes[15].shape, 2);
    EXPECT_EQ(keyframes[16].shape, 11);
    EXPECT_EQ(keyframes[17].shape, 1);
    EXPECT_EQ(keyframes[18].shape, 11);
    EXPECT_EQ(keyframes[19].shape, 7);
    EXPECT_EQ(keyframes[20].shape, 3);
    EXPECT_EQ(keyframes[21].shape, 11);
    EXPECT_EQ(keyframes[22].shape, 0);
    EXPECT_EQ(keyframes[23].shape, 0);
}
