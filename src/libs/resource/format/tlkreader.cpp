/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/resource/format/tlkreader.h"

#include "reone/resource/talktable.h"

using namespace std;

namespace reone {

namespace resource {

struct StringFlags {
    static constexpr int textPresent = 1;
    static constexpr int soundPresent = 2;
    static constexpr int soundLengthPresent = 4;
};

void TlkReader::onLoad() {
    checkSignature(string("TLK V3.0", 8));

    uint32_t languageId = readUint32();
    _stringCount = readUint32();
    _stringsOffset = readUint32();

    loadStrings();
}

void TlkReader::loadStrings() {
    auto strings = vector<TalkTable::String>();

    for (uint32_t i = 0; i < _stringCount; ++i) {
        uint32_t flags = readUint32();

        string soundResRef(readCString(16));
        boost::to_lower(soundResRef);

        ignore(8);

        uint32_t stringOffset = readUint32();
        uint32_t stringSize = readUint32();
        float soundLength = readFloat();

        string text;
        if (flags & StringFlags::textPresent) {
            text = readString(_stringsOffset + stringOffset, stringSize);
        }

        strings.push_back(TalkTable::String {move(text), move(soundResRef)});
    }

    _table = make_unique<TalkTable>(move(strings));
}

} // namespace resource

} // namespace reone
