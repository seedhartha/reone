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

#include "reone/tools/lip/composer.h"

#include "reone/graphics/lipanimation.h"
#include "reone/system/checkutil.h"
#include "reone/system/stringbuilder.h"
#include "reone/system/textreader.h"
#include "reone/system/unicodeutil.h"
#include "reone/tools/types.h"

using namespace reone::graphics;

namespace reone {

static const std::unordered_map<std::string, LipShape> g_phonemeToShape {
    {"aa", LipShape::AA_AE_AH},    //
    {"ae", LipShape::AA_AE_AH},    //
    {"ah", LipShape::AA_AE_AH},    //
    {"ao", LipShape::AO},          //
    {"aw", LipShape::AW_AY},       //
    {"ay", LipShape::AW_AY},       //
    {"b", LipShape::B_M_P},        //
    {"ch", LipShape::CH_JH_SH_ZH}, //
    {"d", LipShape::D_DH_S_Y_Z},   //
    {"dh", LipShape::D_DH_S_Y_Z},  //
    {"eh", LipShape::EH_ER_EY},    //
    {"er", LipShape::EH_ER_EY},    //
    {"ey", LipShape::EH_ER_EY},    //
    {"f", LipShape::F_V},          //
    {"g", LipShape::G_HH_K_NG},    //
    {"hh", LipShape::G_HH_K_NG},   //
    {"ih", LipShape::IH_IY},       //
    {"iy", LipShape::IH_IY},       //
    {"jh", LipShape::CH_JH_SH_ZH}, //
    {"k", LipShape::G_HH_K_NG},    //
    {"l", LipShape::L_N},          //
    {"m", LipShape::B_M_P},        //
    {"n", LipShape::L_N},          //
    {"ng", LipShape::G_HH_K_NG},   //
    {"ow", LipShape::OW_OY},       //
    {"oy", LipShape::OW_OY},       //
    {"p", LipShape::B_M_P},        //
    {"r", LipShape::R},            //
    {"s", LipShape::D_DH_S_Y_Z},   //
    {"sh", LipShape::CH_JH_SH_ZH}, //
    {"t", LipShape::T_TH},         //
    {"th", LipShape::T_TH},        //
    {"uh", LipShape::UH_UW_W},     //
    {"uw", LipShape::UH_UW_W},     //
    {"v", LipShape::F_V},          //
    {"w", LipShape::UH_UW_W},      //
    {"y", LipShape::D_DH_S_Y_Z},   //
    {"z", LipShape::D_DH_S_Y_Z},   //
    {"zh", LipShape::CH_JH_SH_ZH}, //

    // Russian
    {"a", LipShape::AA_AE_AH},      //
    {"c", LipShape::D_DH_S_Y_Z},    //
    {"e", LipShape::EH_ER_EY},      //
    {"h", LipShape::G_HH_K_NG},     //
    {"i", LipShape::IH_IY},         //
    {"j", LipShape::D_DH_S_Y_Z},    //
    {"o", LipShape::AO},            //
    {"sch", LipShape::CH_JH_SH_ZH}, //
    {"u", LipShape::UH_UW_W},       //
};

void PronouncingDictionary::load(IInputStream &stream) {
    auto reader = TextReader(stream);
    while (auto line = reader.readLine()) {
        if (!line) {
            break;
        }
        std::vector<std::string> tokens;
        boost::split(tokens, *line, boost::is_space(), boost::token_compress_on);
        auto word = tokens.front();
        if (*word.rbegin() == ')') {
            // TODO: support variations
            continue;
        }
        tokens.erase(tokens.begin(), tokens.begin() + 1);
        for (auto &token : tokens) {
            boost::to_lower(token);
        }
        _wordToPhonemes.insert({std::move(word), std::move(tokens)});
    }
}

std::unique_ptr<LipAnimation> LipComposer::compose(const std::string &name,
                                                   const std::string &u8Text,
                                                   float duration,
                                                   std::vector<TimeSpan> silentSpans) {
    auto wordGroups = split(u8Text);

    auto spans = std::vector<TimeSpan>();
    if (silentSpans.empty()) {
        spans.push_back(TimeSpan {0.0f, duration});
    } else {
        if (silentSpans.front().startInclusive > 0.0f) {
            spans.push_back(TimeSpan {0.0f, silentSpans.front().startInclusive});
        }
        for (size_t i = 0; i < silentSpans.size() - 1; ++i) {
            spans.push_back(TimeSpan {silentSpans[i].endExclusive, silentSpans[i + 1].startInclusive});
        }
        if (silentSpans.back().endExclusive < duration) {
            spans.push_back(TimeSpan {silentSpans.back().endExclusive, duration});
        }
    }
    if (spans.size() != wordGroups.size()) {
        throw WordGroupsSoundSpansMismatchedException(wordGroups, spans);
    }

    std::vector<LipAnimation::Keyframe> frames;
    for (size_t i = 0; i < wordGroups.size(); ++i) {
        std::vector<LipShape> groupPhonemeShapes;
        for (const auto &word : wordGroups[i]) {
            if (word.empty()) {
                groupPhonemeShapes.push_back(LipShape::Neutral);
                continue;
            }
            auto wordPhonemes = _dict.phonemes(word);
            for (const auto &phoneme : wordPhonemes) {
                auto phonemeCopy = phoneme;
                if (std::isdigit((*phonemeCopy.rbegin()))) {
                    phonemeCopy.pop_back();
                }
                if (phonemeCopy.length() > 1 && boost::ends_with(phonemeCopy, "j")) {
                    // de-pallatize consonants from Russian dictionary
                    phonemeCopy.pop_back();
                }
                if (g_phonemeToShape.count(phonemeCopy) == 0) {
                    throw IllegalPhonemeException(phonemeCopy);
                }
                auto shape = g_phonemeToShape.at(phonemeCopy);
                groupPhonemeShapes.push_back(shape);
            }
        }
        float timePerPhoneme = spans[i].duration() / static_cast<float>(groupPhonemeShapes.size());
        float time = spans[i].startInclusive;
        for (const auto &shape : groupPhonemeShapes) {
            frames.push_back(LipAnimation::Keyframe {time, static_cast<uint8_t>(shape)});
            time += timePerPhoneme;
        }
    }
    for (const auto &span : silentSpans) {
        frames.push_back(LipAnimation::Keyframe {span.startInclusive, static_cast<uint8_t>(LipShape::Neutral)});
        if (span.endExclusive != duration) {
            frames.push_back(LipAnimation::Keyframe {span.endExclusive - 0.00001f, static_cast<uint8_t>(LipShape::Neutral)});
        }
    }
    std::sort(frames.begin(), frames.end(), [](const auto &lhs, const auto &rhs) { return lhs.time < rhs.time; });
    frames.push_back(LipAnimation::Keyframe {duration, static_cast<uint8_t>(LipShape::Neutral)});

    return std::make_unique<LipAnimation>(
        name,
        duration,
        std::move(frames));
}

std::vector<std::vector<std::string>> LipComposer::split(const std::string &u8Text) {
    std::vector<std::vector<std::string>> wordGroups;
    std::vector<std::string> words;
    std::vector<uint32_t> word;

    auto codePoints = codePointsFromUTF8(u8Text);
    int groupStart = -1;
    for (size_t i = 0; i < codePoints.size(); ++i) {
        uint32_t codePoint = codePoints[i];
        if (codePoint == '(') {
            if (groupStart != -1) {
                throw TextSyntaxException(str(boost::format("Unexpected '(' character at %d") % i));
            }
            if (!word.empty()) {
                words.push_back(utf8FromCodePoints(word));
                word.clear();
            }
            if (!words.empty()) {
                wordGroups.push_back(words);
                words.clear();
            }
            groupStart = i;
            continue;
        }
        if (codePoint == ')') {
            if (groupStart == -1) {
                throw TextSyntaxException(str(boost::format("Unexpected ')' character at %d") % i));
            }
            if (!word.empty()) {
                words.push_back(utf8FromCodePoints(word));
                word.clear();
            }
            if (!words.empty()) {
                wordGroups.push_back(words);
                words.clear();
            }
            groupStart = -1;
            continue;
        }
        if (codePoint == '|') {
            if (!word.empty()) {
                words.push_back(utf8FromCodePoints(word));
                word.clear();
            }
            words.push_back("");
            continue;
        }
        if (std::isspace(codePoint)) {
            if (!word.empty()) {
                words.push_back(utf8FromCodePoints(word));
                word.clear();
            }
            continue;
        }
        if (isCodePointAlpha(codePoint)) {
            word.push_back(codePointToLower(codePoint));
            continue;
        }
        if ((codePoint == '\'' || codePoint == '-') && !word.empty()) {
            word.push_back(codePoint);
            continue;
        }
        if (std::ispunct(codePoint)) {
            continue;
        }
        throw TextSyntaxException(str(boost::format("Unexpected UTF-8 code point %04x at %d") % codePoint % i));
    }
    if (groupStart != -1) {
        throw TextSyntaxException(str(boost::format("Expected matching ')' character for '(' at %d") % groupStart));
    }
    if (!word.empty()) {
        words.push_back(utf8FromCodePoints(word));
    }
    if (!words.empty()) {
        wordGroups.push_back(words);
    }

    return wordGroups;
}

} // namespace reone
