#include "tlkfile.h"

#include <boost/algorithm/string.hpp>

namespace reone {

namespace resources {

enum {
    kTextPresent = 1,
    kSoundPresent = 2,
    kSoundLengthPresent = 4
};

const TalkTableString &TalkTable::getString(int32_t ref) const {
    assert(ref > 0 && ref < _strings.size());
    return _strings[ref];
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
    _table = std::make_shared<TalkTable>();
    _table->_strings.reserve(_stringCount);

    for (int i = 0; i < _stringCount; ++i) {
        uint32_t flags = readUint32();

        std::string soundResRef(readFixedString(16));
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

std::shared_ptr<TalkTable> TlkFile::table() const {
    return _table;
}

} // namespace resources

} // namespace reone
