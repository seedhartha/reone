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

TEST(lip_composer, should_compose_lip_file_from_text) {
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
    EXPECT_EQ(keyframes.size(), 11);
    EXPECT_FLOAT_EQ(keyframes[0].time, 0.0f);
    EXPECT_FLOAT_EQ(keyframes[1].time, 0.1f);
    EXPECT_FLOAT_EQ(keyframes[2].time, 0.2f);
    EXPECT_FLOAT_EQ(keyframes[3].time, 0.3f);
    EXPECT_FLOAT_EQ(keyframes[4].time, 0.4f);
    EXPECT_FLOAT_EQ(keyframes[5].time, 0.5f);
    EXPECT_FLOAT_EQ(keyframes[6].time, 0.6f);
    EXPECT_FLOAT_EQ(keyframes[7].time, 0.7f);
    EXPECT_FLOAT_EQ(keyframes[8].time, 0.8f);
    EXPECT_FLOAT_EQ(keyframes[9].time, 0.9f);
    EXPECT_FLOAT_EQ(keyframes[10].time, 1.0f);
    EXPECT_EQ(keyframes[0].shape, 0);
    EXPECT_EQ(keyframes[1].shape, 9);
    EXPECT_EQ(keyframes[2].shape, 3);
    EXPECT_EQ(keyframes[3].shape, 15);
    EXPECT_EQ(keyframes[4].shape, 4);
    EXPECT_EQ(keyframes[5].shape, 0);
    EXPECT_EQ(keyframes[6].shape, 5);
    EXPECT_EQ(keyframes[7].shape, 2);
    EXPECT_EQ(keyframes[8].shape, 15);
    EXPECT_EQ(keyframes[9].shape, 10);
    EXPECT_EQ(keyframes[10].shape, 0);
}
