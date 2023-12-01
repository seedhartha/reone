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

#include "reone/resource/ltr.h"

#include "reone/system/randomutil.h"

namespace reone {

namespace resource {

static const std::vector<char> g_letters {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\'', '-'};

std::string Ltr::randomName(int maxLength) const {
    std::string name;

    float probability;
    int firstLetterIdx = 0;
    int secondLetterIdx = 0;

    // First three letters

    probability = randomFloat(0.0f, 1.0f);
    for (int i = 0; i < _letterCount; ++i) {
        if (_singleLetters.start[i] > probability) {
            name += toupper(g_letters[i]);
            firstLetterIdx = i;
            break;
        }
    }

    probability = randomFloat(0.0f, 1.0f);
    for (int i = 0; i < _letterCount; ++i) {
        if (_doubleLetters[firstLetterIdx].start[i] > probability) {
            name += g_letters[i];
            secondLetterIdx = i;
            break;
        }
    }

    probability = randomFloat(0.0f, 1.0f);
    for (int i = 0; i < _letterCount; ++i) {
        if (_tripleLetters[firstLetterIdx][secondLetterIdx].start[i] > probability) {
            name += g_letters[i];
            firstLetterIdx = secondLetterIdx;
            secondLetterIdx = i;
            break;
        }
    }

    // END First three letters

    if (maxLength < 4)
        return name;

    // Middle letters

    int length = randomInt(0, maxLength - 3);
    for (int i = 0; i < length; ++i) {
        probability = randomFloat(0.0f, 1.0f);
        for (int j = 0; j < _letterCount; ++j) {
            if (_tripleLetters[firstLetterIdx][secondLetterIdx].mid[j] > probability) {
                name += g_letters[j];
                firstLetterIdx = secondLetterIdx;
                secondLetterIdx = j;
                break;
            }
        }
    }

    // END Middle letters

    // Final letter

    probability = randomFloat(0.0f, 1.0f);
    for (int i = 0; i < _letterCount; ++i) {
        if (_tripleLetters[firstLetterIdx][secondLetterIdx].end[i] > probability) {
            name += g_letters[i];
            break;
        }
    }

    // END Final letter

    return name;
}

} // namespace resource

} // namespace reone
