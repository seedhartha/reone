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
#include "reone/tools/lip/composer.h"
#include "reone/tools/types.h"

using namespace reone;

TEST(PronouncingDictionary, should_load_phonemes_from_stream) {
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
    EXPECT_EQ(phonemes, (std::vector<std::string> {"hh", "ah0", "l", "ow1"}));
}

TEST(PronouncingDictionary, should_load_russian_phonemes_from_stream) {
    // given
    auto bytes = ByteBuffer();
    auto ostream = MemoryOutputStream(bytes);
    auto writer = TextWriter(ostream);
    writer.writeLine(u8"привет p rj i0 vj e1 t");
    writer.writeLine(u8"мир mj i1 r");
    auto istream = MemoryInputStream(bytes);
    auto dict = PronouncingDictionary();

    // when
    dict.load(istream);

    // then
    auto phonemes = dict.phonemes(u8"привет");
    EXPECT_EQ(phonemes, (std::vector<std::string> {"p", "rj", "i0", "vj", "e1", "t"}));
}

TEST(LipComposer, should_compose_lip_file_from_text_with_implicit_word_groups_and_empty_silent_spans) {
    // given
    auto wordToPhonemes = PronouncingDictionary::WordPhonemesMap {
        {"hello", {"hh", "ah0", "l", "ow1"}}, //
        {"world", {"w", "er1", "l", "d"}}     //
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
    EXPECT_EQ(keyframes[2].shape, 12);
    EXPECT_EQ(keyframes[3].shape, 4);
    EXPECT_EQ(keyframes[4].shape, 5);
    EXPECT_EQ(keyframes[5].shape, 2);
    EXPECT_EQ(keyframes[6].shape, 12);
    EXPECT_EQ(keyframes[7].shape, 6);
    EXPECT_EQ(keyframes[8].shape, 0);
}

TEST(LipComposer, should_compose_lip_file_from_text_with_explicit_word_groups_and_silent_spans) {
    // given
    auto wordToPhonemes = PronouncingDictionary::WordPhonemesMap {
        {"hello", {"hh", "ah0", "l", "ow1"}},   //
        {"world", {"w", "er1", "l", "d"}},      //
        {"lorem", {"l", "ao", "r", "eh", "m"}}, //
        {"ipsum", {"ih", "p", "s", "ah0", "m"}} //
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
    EXPECT_EQ(keyframes[4].shape, 12);
    EXPECT_EQ(keyframes[5].shape, 4);
    EXPECT_EQ(keyframes[6].shape, 5);
    EXPECT_EQ(keyframes[7].shape, 2);
    EXPECT_EQ(keyframes[8].shape, 12);
    EXPECT_EQ(keyframes[9].shape, 6);
    EXPECT_EQ(keyframes[10].shape, 0);
    EXPECT_EQ(keyframes[11].shape, 0);
    EXPECT_EQ(keyframes[12].shape, 12);
    EXPECT_EQ(keyframes[13].shape, 15);
    EXPECT_EQ(keyframes[14].shape, 13);
    EXPECT_EQ(keyframes[15].shape, 2);
    EXPECT_EQ(keyframes[16].shape, 11);
    EXPECT_EQ(keyframes[17].shape, 1);
    EXPECT_EQ(keyframes[18].shape, 11);
    EXPECT_EQ(keyframes[19].shape, 6);
    EXPECT_EQ(keyframes[20].shape, 3);
    EXPECT_EQ(keyframes[21].shape, 11);
    EXPECT_EQ(keyframes[22].shape, 0);
    EXPECT_EQ(keyframes[23].shape, 0);
}

TEST(LipComposer, should_compose_lip_file_from_text_with_all_phonemes) {
    // given
    auto wordToPhonemes = PronouncingDictionary::WordPhonemesMap {{
        "all", {"aa", "ae", "ah", "ao", "aw", "ay", "b", "ch", "d", "dh", //
                "eh", "er", "ey", "f", "g", "hh", "ih", "iy", "jh", "k",  //
                "l", "m", "n", "ng", "ow", "oy", "p", "r", "s", "sh",     //
                "t", "th", "uh", "uw", "v", "w", "y", "z", "zh"}          //
    }};
    auto dict = PronouncingDictionary(wordToPhonemes);
    auto composer = LipComposer(dict);

    // when
    auto anim = composer.compose("name", "all", 39.0f);

    // then
    EXPECT_TRUE(anim);
    EXPECT_EQ(anim->name(), "name");
    EXPECT_EQ(anim->length(), 39.0f);
    auto &keyframes = anim->keyframes();
    EXPECT_EQ(keyframes.size(), 40);
    EXPECT_FLOAT_EQ(keyframes[0].time, 0.0f);
    EXPECT_FLOAT_EQ(keyframes[1].time, 1.0f);
    EXPECT_FLOAT_EQ(keyframes[2].time, 2.0f);
    EXPECT_FLOAT_EQ(keyframes[3].time, 3.0f);
    EXPECT_FLOAT_EQ(keyframes[4].time, 4.0f);
    EXPECT_FLOAT_EQ(keyframes[5].time, 5.0f);
    EXPECT_FLOAT_EQ(keyframes[6].time, 6.0f);
    EXPECT_FLOAT_EQ(keyframes[7].time, 7.0f);
    EXPECT_FLOAT_EQ(keyframes[8].time, 8.0f);
    EXPECT_FLOAT_EQ(keyframes[9].time, 9.0f);
    EXPECT_FLOAT_EQ(keyframes[10].time, 10.0f);
    EXPECT_FLOAT_EQ(keyframes[11].time, 11.0f);
    EXPECT_FLOAT_EQ(keyframes[12].time, 12.0f);
    EXPECT_FLOAT_EQ(keyframes[13].time, 13.0f);
    EXPECT_FLOAT_EQ(keyframes[14].time, 14.0f);
    EXPECT_FLOAT_EQ(keyframes[15].time, 15.0f);
    EXPECT_FLOAT_EQ(keyframes[16].time, 16.0f);
    EXPECT_FLOAT_EQ(keyframes[17].time, 17.0f);
    EXPECT_FLOAT_EQ(keyframes[18].time, 18.0f);
    EXPECT_FLOAT_EQ(keyframes[19].time, 19.0f);
    EXPECT_FLOAT_EQ(keyframes[20].time, 20.0f);
    EXPECT_FLOAT_EQ(keyframes[21].time, 21.0f);
    EXPECT_FLOAT_EQ(keyframes[22].time, 22.0f);
    EXPECT_FLOAT_EQ(keyframes[23].time, 23.0f);
    EXPECT_FLOAT_EQ(keyframes[24].time, 24.0f);
    EXPECT_FLOAT_EQ(keyframes[25].time, 25.0f);
    EXPECT_FLOAT_EQ(keyframes[26].time, 26.0f);
    EXPECT_FLOAT_EQ(keyframes[27].time, 27.0f);
    EXPECT_FLOAT_EQ(keyframes[28].time, 28.0f);
    EXPECT_FLOAT_EQ(keyframes[29].time, 29.0f);
    EXPECT_FLOAT_EQ(keyframes[30].time, 30.0f);
    EXPECT_FLOAT_EQ(keyframes[31].time, 31.0f);
    EXPECT_FLOAT_EQ(keyframes[32].time, 32.0f);
    EXPECT_FLOAT_EQ(keyframes[33].time, 33.0f);
    EXPECT_FLOAT_EQ(keyframes[34].time, 34.0f);
    EXPECT_FLOAT_EQ(keyframes[35].time, 35.0f);
    EXPECT_FLOAT_EQ(keyframes[36].time, 36.0f);
    EXPECT_FLOAT_EQ(keyframes[37].time, 37.0f);
    EXPECT_FLOAT_EQ(keyframes[38].time, 38.0f);
    EXPECT_FLOAT_EQ(keyframes[39].time, 39.0f);
    EXPECT_EQ(keyframes[0].shape, 3);
    EXPECT_EQ(keyframes[1].shape, 3);
    EXPECT_EQ(keyframes[2].shape, 3);
    EXPECT_EQ(keyframes[3].shape, 15);
    EXPECT_EQ(keyframes[4].shape, 14);
    EXPECT_EQ(keyframes[5].shape, 14);
    EXPECT_EQ(keyframes[6].shape, 11);
    EXPECT_EQ(keyframes[7].shape, 7);
    EXPECT_EQ(keyframes[8].shape, 6);
    EXPECT_EQ(keyframes[9].shape, 6);
    EXPECT_EQ(keyframes[10].shape, 2);
    EXPECT_EQ(keyframes[11].shape, 2);
    EXPECT_EQ(keyframes[12].shape, 2);
    EXPECT_EQ(keyframes[13].shape, 8);
    EXPECT_EQ(keyframes[14].shape, 9);
    EXPECT_EQ(keyframes[15].shape, 9);
    EXPECT_EQ(keyframes[16].shape, 1);
    EXPECT_EQ(keyframes[17].shape, 1);
    EXPECT_EQ(keyframes[18].shape, 7);
    EXPECT_EQ(keyframes[19].shape, 9);
    EXPECT_EQ(keyframes[20].shape, 12);
    EXPECT_EQ(keyframes[21].shape, 11);
    EXPECT_EQ(keyframes[22].shape, 12);
    EXPECT_EQ(keyframes[23].shape, 9);
    EXPECT_EQ(keyframes[24].shape, 4);
    EXPECT_EQ(keyframes[25].shape, 4);
    EXPECT_EQ(keyframes[26].shape, 11);
    EXPECT_EQ(keyframes[27].shape, 13);
    EXPECT_EQ(keyframes[28].shape, 6);
    EXPECT_EQ(keyframes[29].shape, 7);
    EXPECT_EQ(keyframes[30].shape, 10);
    EXPECT_EQ(keyframes[31].shape, 10);
    EXPECT_EQ(keyframes[32].shape, 5);
    EXPECT_EQ(keyframes[33].shape, 5);
    EXPECT_EQ(keyframes[34].shape, 8);
    EXPECT_EQ(keyframes[35].shape, 5);
    EXPECT_EQ(keyframes[36].shape, 6);
    EXPECT_EQ(keyframes[37].shape, 6);
    EXPECT_EQ(keyframes[38].shape, 7);
    EXPECT_EQ(keyframes[39].shape, 0);
}

TEST(LipComposer, should_compose_lip_file_from_text_with_explicit_pauses) {
    // given
    auto wordToPhonemes = PronouncingDictionary::WordPhonemesMap {
        {"hello", {"hh", "ah0", "l", "ow1"}}, //
        {"world", {"w", "er1", "l", "d"}}     //
    };
    auto dict = PronouncingDictionary(wordToPhonemes);
    auto composer = LipComposer(dict);

    // when
    auto anim = composer.compose("name", "Hello, | world!", 1.0f);

    // then
    EXPECT_TRUE(anim);
    EXPECT_EQ(anim->name(), "name");
    EXPECT_EQ(anim->length(), 1.0f);
    auto &keyframes = anim->keyframes();
    EXPECT_EQ(keyframes.size(), 10);
    EXPECT_FLOAT_EQ(keyframes[0].time, 0.0f);
    EXPECT_NEAR(keyframes[1].time, 0.111f, 0.001f);
    EXPECT_NEAR(keyframes[2].time, 0.222f, 0.001f);
    EXPECT_NEAR(keyframes[3].time, 0.333f, 0.001f);
    EXPECT_NEAR(keyframes[4].time, 0.444f, 0.001f);
    EXPECT_NEAR(keyframes[5].time, 0.555f, 0.001f);
    EXPECT_NEAR(keyframes[6].time, 0.666f, 0.001f);
    EXPECT_NEAR(keyframes[7].time, 0.777f, 0.001f);
    EXPECT_NEAR(keyframes[8].time, 0.888f, 0.001f);
    EXPECT_FLOAT_EQ(keyframes[9].time, 1.0f);
    EXPECT_EQ(keyframes[0].shape, 9);
    EXPECT_EQ(keyframes[1].shape, 3);
    EXPECT_EQ(keyframes[2].shape, 12);
    EXPECT_EQ(keyframes[3].shape, 4);
    EXPECT_EQ(keyframes[4].shape, 0);
    EXPECT_EQ(keyframes[5].shape, 5);
    EXPECT_EQ(keyframes[6].shape, 2);
    EXPECT_EQ(keyframes[7].shape, 12);
    EXPECT_EQ(keyframes[8].shape, 6);
    EXPECT_EQ(keyframes[9].shape, 0);
}

TEST(LipComposer, should_compose_lip_file_from_russian_text) {
    // given
    auto wordToPhonemes = PronouncingDictionary::WordPhonemesMap {
        {u8"all", {
                      "a", "b", "c", "ch",  //
                      "d", "e", "f", "g",   //
                      "h", "i", "j", "k",   //
                      "l", "m", "n", "o",   //
                      "p", "r", "s", "sch", //
                      "sh", "t", "u", "v",  //
                      "y", "z", "zh"        //
                  }}};
    auto dict = PronouncingDictionary(wordToPhonemes);
    auto composer = LipComposer(dict);

    // when
    auto anim = composer.compose("name", u8"all", 1.0f);

    // then
    EXPECT_TRUE(anim);
    EXPECT_EQ(anim->name(), "name");
    EXPECT_EQ(anim->length(), 1.0f);
    auto &keyframes = anim->keyframes();
    EXPECT_EQ(keyframes.size(), 28);
    EXPECT_EQ(keyframes[0].shape, static_cast<uint8_t>(LipShape::AA_AE_AH));
    EXPECT_EQ(keyframes[1].shape, static_cast<uint8_t>(LipShape::B_M_P));
    EXPECT_EQ(keyframes[2].shape, static_cast<uint8_t>(LipShape::D_DH_S_Y_Z));
    EXPECT_EQ(keyframes[3].shape, static_cast<uint8_t>(LipShape::CH_JH_SH_ZH));
    EXPECT_EQ(keyframes[4].shape, static_cast<uint8_t>(LipShape::D_DH_S_Y_Z));
    EXPECT_EQ(keyframes[5].shape, static_cast<uint8_t>(LipShape::EH_ER_EY));
    EXPECT_EQ(keyframes[6].shape, static_cast<uint8_t>(LipShape::F_V));
    EXPECT_EQ(keyframes[7].shape, static_cast<uint8_t>(LipShape::G_HH_K_NG));
    EXPECT_EQ(keyframes[8].shape, static_cast<uint8_t>(LipShape::G_HH_K_NG));
    EXPECT_EQ(keyframes[9].shape, static_cast<uint8_t>(LipShape::IH_IY));
    EXPECT_EQ(keyframes[10].shape, static_cast<uint8_t>(LipShape::D_DH_S_Y_Z));
    EXPECT_EQ(keyframes[11].shape, static_cast<uint8_t>(LipShape::G_HH_K_NG));
    EXPECT_EQ(keyframes[12].shape, static_cast<uint8_t>(LipShape::L_N));
    EXPECT_EQ(keyframes[13].shape, static_cast<uint8_t>(LipShape::B_M_P));
    EXPECT_EQ(keyframes[14].shape, static_cast<uint8_t>(LipShape::L_N));
    EXPECT_EQ(keyframes[15].shape, static_cast<uint8_t>(LipShape::AO));
    EXPECT_EQ(keyframes[16].shape, static_cast<uint8_t>(LipShape::B_M_P));
    EXPECT_EQ(keyframes[17].shape, static_cast<uint8_t>(LipShape::R));
    EXPECT_EQ(keyframes[18].shape, static_cast<uint8_t>(LipShape::D_DH_S_Y_Z));
    EXPECT_EQ(keyframes[19].shape, static_cast<uint8_t>(LipShape::CH_JH_SH_ZH));
    EXPECT_EQ(keyframes[20].shape, static_cast<uint8_t>(LipShape::CH_JH_SH_ZH));
    EXPECT_EQ(keyframes[21].shape, static_cast<uint8_t>(LipShape::T_TH));
    EXPECT_EQ(keyframes[22].shape, static_cast<uint8_t>(LipShape::UH_UW_W));
    EXPECT_EQ(keyframes[23].shape, static_cast<uint8_t>(LipShape::F_V));
    EXPECT_EQ(keyframes[24].shape, static_cast<uint8_t>(LipShape::D_DH_S_Y_Z));
    EXPECT_EQ(keyframes[25].shape, static_cast<uint8_t>(LipShape::D_DH_S_Y_Z));
    EXPECT_EQ(keyframes[26].shape, static_cast<uint8_t>(LipShape::CH_JH_SH_ZH));
    EXPECT_EQ(keyframes[27].shape, 0);
}
