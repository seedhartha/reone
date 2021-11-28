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

#include "talktable.h"

using namespace std;

namespace reone {

namespace resource {

void TalkTable::addString(TalkTableString &&string) {
    _strings.push_back(string);
}

int TalkTable::getStringCount() const {
    return static_cast<int>(_strings.size());
}

const TalkTableString &TalkTable::getString(int index) const {
    if (index < 0 || index >= static_cast<int>(_strings.size())) {
        throw out_of_range("index is out of range");
    }
    return _strings[index];
}

} // namespace resource

} // namespace reone
