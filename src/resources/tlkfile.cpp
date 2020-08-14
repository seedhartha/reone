/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "tlkfile.h"

#include <boost/algorithm/string.hpp>

using namespace std;

namespace reone {

namespace resources {

enum {
    kTextPresent = 1,
    kSoundPresent = 2,
    kSoundLengthPresent = 4
};

const TalkTableString &TalkTable::getString(int32_t ref) const {
    assert(ref >= 0 && ref < _strings.size());
    return _strings[ref];
}

int TalkTable::stringCount() const {
    return static_cast<int>(_strings.size());
}

TlkFile::TlkFile() : BinaryFile(8, "TLK V3.0") {
}

void TlkFile::doLoad() {
    uint32_t languageId = readUint32();
    _stringCount = readUint32();
    _stringsOffset = readUint32();

    loadStrings();
}

void TlkFile::loadStrings() {
    _table = make_shared<TalkTable>();
    _table->_strings.reserve(_stringCount);

    for (uint32_t i = 0; i < _stringCount; ++i) {
        uint32_t flags = readUint32();

        string soundResRef(readFixedString(16));
        boost::to_lower(soundResRef);

        ignore(8);

        uint32_t stringOffset = readUint32();
        uint32_t stringSize = readUint32();
        float soundLength = readFloat();

        TalkTableString string;
        if (flags & kTextPresent) {
            string.text = readString(_stringsOffset + stringOffset, stringSize);
        }
        if (flags & kSoundPresent) {
            string.soundResRef = soundResRef;
        }

        _table->_strings.push_back(string);
    }
}

shared_ptr<TalkTable> TlkFile::table() const {
    return _table;
}

} // namespace resources

} // namespace reone
