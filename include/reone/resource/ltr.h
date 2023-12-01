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

namespace reone {

namespace resource {

class Ltr : boost::noncopyable {
public:
    struct LetterSet {
        std::vector<float> start;
        std::vector<float> mid;
        std::vector<float> end;
    };

    Ltr(int letterCount,
        LetterSet singleLetters,
        std::vector<LetterSet> doubleLetters,
        std::vector<std::vector<LetterSet>> tripleLetters) :
        _letterCount(letterCount),
        _singleLetters(std::move(singleLetters)),
        _doubleLetters(std::move(doubleLetters)),
        _tripleLetters(std::move(tripleLetters)) {
    }

    std::string randomName(int maxLength) const;

private:
    int _letterCount;
    LetterSet _singleLetters;
    std::vector<LetterSet> _doubleLetters;
    std::vector<std::vector<LetterSet>> _tripleLetters;
};

} // namespace resource

} // namespace reone
