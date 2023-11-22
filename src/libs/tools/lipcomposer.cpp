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

// AA   odd     AA D
// AE   at      AE T
// AH   hut     HH AH T
// AO   ought   AO T
// AW   cow     K AW
// AY   hide    HH AY D
// B    be      B IY
// CH   cheese  CH IY Z
// D    dee     D IY
// DH   thee    DH IY
// EH   Ed      EH D
// ER   hurt    HH ER T
// EY   ate     EY T
// F    fee     F IY
// G    green   G R IY N
// HH   he      HH IY
// IH   it      IH T
// IY   eat     IY T
// JH   gee     JH IY
// K    key     K IY
// L    lee     L IY
// M    me      M IY
// N    knee    N IY
// NG   ping    P IH NG
// OW   oat     OW T
// OY   toy     T OY
// P    pee     P IY
// R    read    R IY D
// S    sea     S IY
// SH   she     SH IY
// T    tea     T IY
// TH   theta   TH EY T AH
// UH   hood    HH UH D
// UW   two     T UW
// V    vee     V IY
// W    we      W IY
// Y    yield   Y IY L D
// Z    zee     Z IY
// ZH   seizure S IY ZH ER

#include "reone/tools/lipcomposer.h"

#include "reone/graphics/lipanimation.h"
#include "reone/system/stringbuilder.h"
#include "reone/system/textreader.h"

using namespace reone::graphics;

namespace reone {

enum class Shape : uint8_t {
    Rest = 0,
    IH = 1,
    EH_ER_IY = 2,
    AA_AH_AW_AY = 3,
    AO_OW_R = 4,
    OY_UH_UW_W = 5,
    Y = 6,
    S = 7,
    F_V = 8,
    G_HH_K = 9,
    D_DH_T_TH = 10,
    B_M_P = 11,
    N_NG = 12,
    CH_JH_SH_ZH = 13,
    AE_EY = 14,
    L_Z = 15
};

static const std::unordered_map<std::string, Shape> g_phonemeToShape {
    {"aa", Shape::AA_AH_AW_AY}, //
    {"ae", Shape::AE_EY},       //
    {"ah", Shape::AA_AH_AW_AY}, //
    {"ao", Shape::AO_OW_R},     //
    {"aw", Shape::AA_AH_AW_AY}, //
    {"ay", Shape::AA_AH_AW_AY}, //
    {"b", Shape::B_M_P},        //
    {"ch", Shape::CH_JH_SH_ZH}, //
    {"d", Shape::D_DH_T_TH},    //
    {"dh", Shape::D_DH_T_TH},   //
    {"eh", Shape::EH_ER_IY},    //
    {"er", Shape::EH_ER_IY},    //
    {"ey", Shape::AE_EY},       //
    {"f", Shape::F_V},          //
    {"g", Shape::G_HH_K},       //
    {"hh", Shape::G_HH_K},      //
    {"ih", Shape::IH},          //
    {"iy", Shape::EH_ER_IY},    //
    {"jh", Shape::CH_JH_SH_ZH}, //
    {"k", Shape::G_HH_K},       //
    {"l", Shape::L_Z},          //
    {"m", Shape::B_M_P},        //
    {"n", Shape::N_NG},         //
    {"ng", Shape::N_NG},        //
    {"ow", Shape::AO_OW_R},     //
    {"oy", Shape::OY_UH_UW_W},  //
    {"p", Shape::B_M_P},        //
    {"r", Shape::AO_OW_R},      //
    {"s", Shape::S},            //
    {"sh", Shape::CH_JH_SH_ZH}, //
    {"t", Shape::D_DH_T_TH},    //
    {"th", Shape::D_DH_T_TH},   //
    {"uh", Shape::OY_UH_UW_W},  //
    {"uw", Shape::OY_UH_UW_W},  //
    {"v", Shape::F_V},          //
    {"w", Shape::OY_UH_UW_W},   //
    {"y", Shape::Y},            //
    {"z", Shape::L_Z},          //
    {"zh", Shape::CH_JH_SH_ZH}, //
};

struct WordDurationTuple {
    std::string word;
    float start {0.0f};
    float end {0.0f};
};

struct TimeSpanPhonemesPair {
    TimeSpan span;
    std::vector<std::string> phonemes;
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
            continue;
        }
        tokens.erase(tokens.begin(), tokens.begin() + 1);
        _wordToPhonemes.insert(std::make_pair(word, tokens));
    }
}

std::unique_ptr<LipAnimation> LipComposer::compose(const std::string &name,
                                                   const std::string &text,
                                                   float duration,
                                                   std::vector<TimeSpan> silentSpans) {
    auto wordGroups = split(text);

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
        std::vector<Shape> groupPhonemeShapes;
        for (const auto &word : wordGroups[i]) {
            auto wordPhonemes = _dict.phonemes(word);
            for (const auto &phoneme : wordPhonemes) {
                auto lowerPhoneme = boost::to_lower_copy(phoneme);
                if (std::isdigit((*lowerPhoneme.rbegin()))) {
                    lowerPhoneme.pop_back();
                }
                if (g_phonemeToShape.count(lowerPhoneme) == 0) {
                    throw IllegalPhonemeException(lowerPhoneme);
                }
                auto shape = g_phonemeToShape.at(lowerPhoneme);
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
        frames.push_back(LipAnimation::Keyframe {span.startInclusive, static_cast<uint8_t>(Shape::Rest)});
    }
    std::sort(frames.begin(), frames.end(), [](const auto &lhs, const auto &rhs) { return lhs.time < rhs.time; });
    frames.push_back(LipAnimation::Keyframe {duration, static_cast<uint8_t>(Shape::Rest)});

    return std::make_unique<LipAnimation>(
        name,
        duration,
        std::move(frames));
}

std::vector<std::vector<std::string>> LipComposer::split(const std::string &text) {
    std::vector<std::vector<std::string>> wordGroups;
    std::vector<std::string> words;
    StringBuilder word;

    int groupStart = -1;
    for (size_t i = 0; i < text.length(); ++i) {
        const auto &ch = text[i];
        if (ch == '(') {
            if (groupStart != -1) {
                throw TextSyntaxException(str(boost::format("Unexpected '(' character at %d") % i));
            }
            if (!word.empty()) {
                words.push_back(word.string());
                word.clear();
            }
            if (!words.empty()) {
                wordGroups.push_back(words);
                words.clear();
            }
            groupStart = i;
            continue;
        }
        if (ch == ')') {
            if (groupStart == -1) {
                throw TextSyntaxException(str(boost::format("Unexpected ')' character at %d") % i));
            }
            if (!word.empty()) {
                words.push_back(word.string());
                word.clear();
            }
            if (!words.empty()) {
                wordGroups.push_back(words);
                words.clear();
            }
            groupStart = -1;
            continue;
        }
        if (std::isspace(ch)) {
            if (!word.empty()) {
                words.push_back(word.string());
                word.clear();
            }
            continue;
        }
        if (std::isalpha(ch) || ((ch == '\'' || ch == '-') && !word.empty())) {
            word.append(std::tolower(ch));
            continue;
        }
        if (std::ispunct(ch)) {
            continue;
        }
        throw TextSyntaxException(str(boost::format("Unexpected character %d at %d") % static_cast<int>(ch) % i));
    }
    if (groupStart != -1) {
        throw TextSyntaxException(str(boost::format("Expected matching ')' character for '(' at %d") % groupStart));
    }
    if (!word.empty()) {
        words.push_back(word.string());
    }
    if (!words.empty()) {
        wordGroups.push_back(words);
    }

    return wordGroups;
}

} // namespace reone
