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

#include "reone/system/binaryreader.h"
#include "reone/system/stream/input.h"

namespace reone {

namespace resource {

/**
 * LTR contains rules for random name generation.
 */
class LtrReader : boost::noncopyable {
public:
    LtrReader(IInputStream &ltr) :
        _ltr(BinaryReader(ltr)) {
    }

    void load();

    std::string getRandomName(int maxLength) const;

private:
    struct LetterSet {
        std::vector<float> start;
        std::vector<float> mid;
        std::vector<float> end;
    };

    BinaryReader _ltr;

    int _letterCount {0};
    LetterSet _singleLetters;
    std::vector<LetterSet> _doubleLetters;
    std::vector<std::vector<LetterSet>> _trippleLetters;

    void readLetterSet(LetterSet &set);
};

} // namespace resource

} // namespace reone
