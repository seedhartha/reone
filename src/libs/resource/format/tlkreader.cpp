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

#include "reone/resource/format/tlkreader.h"

#include "reone/resource/talktable.h"
#include "reone/system/checkutil.h"

namespace reone {

namespace resource {

struct StringFlags {
    static constexpr int textPresent = 1;
    static constexpr int soundPresent = 2;
    static constexpr int soundLengthPresent = 4;
};

void TlkReader::load() {
    checkEqual("TLK signature", _tlk.readString(8), std::string("TLK V3.0", 8));

    uint32_t languageId = _tlk.readUint32();
    _stringCount = _tlk.readUint32();
    _stringsOffset = _tlk.readUint32();

    loadStrings();
}

void TlkReader::loadStrings() {
    auto strings = std::vector<TalkTable::String>();

    for (uint32_t i = 0; i < _stringCount; ++i) {
        uint32_t flags = _tlk.readUint32();

        std::string soundResRef(_tlk.readString(16));
        boost::to_lower(soundResRef);

        _tlk.skipBytes(8);

        uint32_t stringOffset = _tlk.readUint32();
        uint32_t stringSize = _tlk.readUint32();
        float soundLength = _tlk.readFloat();

        std::string text;
        if (flags & StringFlags::textPresent) {
            text = _tlk.readStringAt(_stringsOffset + stringOffset, stringSize);
        }

        strings.push_back(TalkTable::String {std::move(text), std::move(soundResRef)});
    }

    _table = std::make_unique<TalkTable>(std::move(strings));
}

} // namespace resource

} // namespace reone
