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

#pragma once

#include "reone/system/stream/input.h"

namespace reone {

namespace graphics {

class LipAnimation;

}

class WordPhonemesNotFoundException : public std::runtime_error {
public:
    WordPhonemesNotFoundException(std::string word) :
        std::runtime_error(str(boost::format("Phonemes not found for word '%s'") % word)),
        _word(std::move(word)) {
    }

    const std::string &word() const {
        return _word;
    }

private:
    std::string _word;
};

class PronouncingDictionary {
public:
    typedef std::unordered_map<std::string, std::vector<std::string>> WordPhonemesMap;

    PronouncingDictionary() = default;

    PronouncingDictionary(WordPhonemesMap wordToPhonemes) :
        _wordToPhonemes(std::move(wordToPhonemes)) {
    }

    void load(IInputStream &stream);

    /**
     * @throws WordPhonemesNotFoundException
     */
    std::vector<std::string> phonemes(const std::string &word) const {
        auto lowerWord = boost::to_lower_copy(word);
        if (_wordToPhonemes.count(lowerWord) == 0) {
            throw WordPhonemesNotFoundException(lowerWord);
        }
        return _wordToPhonemes.at(lowerWord);
    }

private:
    WordPhonemesMap _wordToPhonemes;
};

class IllegalPhonemeException : public std::runtime_error {
public:
    IllegalPhonemeException(std::string phoneme) :
        std::runtime_error(str(boost::format("Illegal phoneme '%s'") % phoneme)),
        _phoneme(std::move(phoneme)) {
    }

private:
    std::string _phoneme;
};

class LipComposer : boost::noncopyable {
public:
    LipComposer(PronouncingDictionary dict) :
        _dict(std::move(dict)) {
    }

    /**
     * @throws WordPhonemesNotFoundException
     * @throws IllegalPhonemeException
     */
    std::unique_ptr<graphics::LipAnimation> compose(const std::string &name,
                                                    const std::string &text,
                                                    float length = 1.0f);

private:
    PronouncingDictionary _dict;

    std::vector<std::string> split(const std::string &text);
};

} // namespace reone
