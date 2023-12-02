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

#include "reone/resource/format/ltrreader.h"

#include "reone/resource/format/signutil.h"
#include "reone/system/exception/validation.h"

namespace reone {

namespace resource {

std::unique_ptr<Ltr> LtrReader::load() {
    checkSignature(_ltr, std::string("LTR V1.0", 8));

    int letterCount = _ltr.readByte();
    if (letterCount != 28) {
        throw ValidationException("Invalid letter count: " + std::to_string(letterCount));
    }
    Ltr::LetterSet singleLetters;
    readLetterSet(letterCount, singleLetters);

    std::vector<Ltr::LetterSet> doubleLetters;
    doubleLetters.resize(letterCount);
    for (auto &set : doubleLetters) {
        readLetterSet(letterCount, set);
    }

    std::vector<std::vector<Ltr::LetterSet>> tripleLetters;
    tripleLetters.resize(letterCount);
    for (auto &sets : tripleLetters) {
        sets.resize(letterCount);
        for (auto &set : sets) {
            readLetterSet(letterCount, set);
        }
    }

    return std::make_unique<Ltr>(
        letterCount,
        std::move(singleLetters),
        std::move(doubleLetters),
        std::move(tripleLetters));
}

void LtrReader::readLetterSet(int count, Ltr::LetterSet &set) {
    set.start.resize(count);
    for (int i = 0; i < count; ++i) {
        set.start[i] = _ltr.readFloat();
    }

    set.mid.resize(count);
    for (int i = 0; i < count; ++i) {
        set.mid[i] = _ltr.readFloat();
    }

    set.end.resize(count);
    for (int i = 0; i < count; ++i) {
        set.end[i] = _ltr.readFloat();
    }
}

} // namespace resource

} // namespace reone
