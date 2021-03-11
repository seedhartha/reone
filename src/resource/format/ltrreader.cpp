/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "ltrreader.h"

#include <cctype>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../../common/random.h"

using namespace std;

namespace reone {

namespace resource {

static const vector<char> g_letters {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\'', '-'
};

LtrReader::LtrReader() : BinaryReader(8, "LTR V1.0") {
}

void LtrReader::doLoad() {
    _letterCount = readByte();
    if (_letterCount != 28) {
        throw runtime_error("Unsupported letter count");
    }
    readLetterSet(_singleLetters);

    _doubleLetters.resize(_letterCount);
    for (auto &set : _doubleLetters) {
        readLetterSet(set);
    }

    _trippleLetters.resize(_letterCount);
    for (auto &sets : _trippleLetters) {
        sets.resize(_letterCount);
        for (auto &set : sets) {
            readLetterSet(set);
        }
    }
}

void LtrReader::readLetterSet(LetterSet &set) {
    set.start.resize(_letterCount);
    for (int i = 0; i < _letterCount; ++i) {
        set.start[i] = readFloat();
    }

    set.mid.resize(_letterCount);
    for (int i = 0; i < _letterCount; ++i) {
        set.mid[i] = readFloat();
    }

    set.end.resize(_letterCount);
    for (int i = 0; i < _letterCount; ++i) {
        set.end[i] = readFloat();
    }
}

string LtrReader::getRandomName(int maxLength) const {
    string name;

    float probability;
    int firstLetterIdx = 0;
    int secondLetterIdx = 0;

    // First three letters

    probability = random(0.0f, 1.0f);
    for (int i = 0; i < _letterCount; ++i) {
        if (_singleLetters.start[i] > probability) {
            name += toupper(g_letters[i]);
            firstLetterIdx = i;
            break;
        }
    }

    probability = random(0.0f, 1.0f);
    for (int i = 0; i < _letterCount; ++i) {
        if (_doubleLetters[firstLetterIdx].start[i] > probability) {
            name += g_letters[i];
            secondLetterIdx = i;
            break;
        }
    }

    probability = random(0.0f, 1.0f);
    for (int i = 0; i < _letterCount; ++i) {
        if (_trippleLetters[firstLetterIdx][secondLetterIdx].start[i] > probability) {
            name += g_letters[i];
            firstLetterIdx = secondLetterIdx;
            secondLetterIdx = i;
            break;
        }
    }

    // END First three letters

    if (maxLength < 4) return move(name);

    // Middle letters

    int length = random(0, maxLength - 3);
    for (int i = 0; i < length; ++i) {
        probability = random(0.0f, 1.0f);
        for (int j = 0; j < _letterCount; ++j) {
            if (_trippleLetters[firstLetterIdx][secondLetterIdx].mid[j] > probability) {
                name += g_letters[j];
                firstLetterIdx = secondLetterIdx;
                secondLetterIdx = j;
                break;
            }
        }
    }

    // END Middle letters

    // Final letter

    probability = random(0.0f, 1.0f);
    for (int i = 0; i < _letterCount; ++i) {
        if (_trippleLetters[firstLetterIdx][secondLetterIdx].end[i] > probability) {
            name += g_letters[i];
            break;
        }
    }

    // END Final letter

    return move(name);
}

} // namespace resource

} // namespace reone
