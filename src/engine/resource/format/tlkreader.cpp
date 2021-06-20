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

#include "tlkreader.h"

#include "../talktable.h"

using namespace std;

namespace reone {

namespace resource {

struct StringFlags {
    static constexpr int textPresent = 1;
    static constexpr int soundPresent = 2;
    static constexpr int soundLengthPresent = 4;
};

TlkReader::TlkReader() : BinaryReader(8, "TLK V3.0") {
}

void TlkReader::doLoad() {
    uint32_t languageId = readUint32();
    _stringCount = readUint32();
    _stringsOffset = readUint32();

    loadStrings();
}

void TlkReader::loadStrings() {
    _table = make_shared<TalkTable>();

    for (uint32_t i = 0; i < _stringCount; ++i) {
        uint32_t flags = readUint32();

        string soundResRef(readCString(16));
        boost::to_lower(soundResRef);

        ignore(8);

        uint32_t stringOffset = readUint32();
        uint32_t stringSize = readUint32();
        float soundLength = readFloat();

        TalkTableString tableString;
        if (flags & StringFlags::textPresent) {
            tableString.text = readString(_stringsOffset + stringOffset, stringSize);
        }
        if (flags & StringFlags::soundPresent) {
            tableString.soundResRef = soundResRef;
        }
        _table->addString(move(tableString));
    }
}

} // namespace resource

} // namespace reone
