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

#include "tlkwriter.h"

#include <stdexcept>

#include <boost/filesystem.hpp>

#include "../../common/streamwriter.h"
#include "../../common/stringutil.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

struct StringFlags {
    static constexpr int textPresent = 1;
    static constexpr int soundPresent = 2;
    static constexpr int soundLengthPresent = 4;
};

TlkWriter::TlkWriter(shared_ptr<TalkTable> talkTable) : _talkTable(talkTable) {
    if (!talkTable) {
        throw invalid_argument("talkTable must not be null");
    }
}

void TlkWriter::save(const fs::path &path) {
    vector<StringDataElement> strData;

    uint32_t offString = 0;
    for (int i = 0; i < _talkTable->getStringCount(); ++i) {
        const TalkTableString &str = _talkTable->getString(i);
        auto strSize = static_cast<uint32_t>(str.text.length());

        StringDataElement strDataElem;
        strDataElem.flags = 7; // all strings in dialog.tlk have this
        strncpy(strDataElem.soundResRef, str.soundResRef.c_str(), str.soundResRef.length());
        memset(strDataElem.soundResRef, 0, 16);
        strDataElem.offString = offString;
        strDataElem.stringSize = strSize;
        strData.push_back(move(strDataElem));

        offString += strSize + 1;
    }

    auto tlk = make_shared<fs::ofstream>(path, ios::binary);
    StreamWriter writer(tlk);

    FileHeader fileHeader;
    fileHeader.languageId = 0;
    fileHeader.numStrings = _talkTable->getStringCount();
    fileHeader.offStringEntries = 8 + sizeof(FileHeader) + _talkTable->getStringCount() * sizeof(StringDataElement);

    writer.putString("TLK V3.0");
    writer.putStruct(fileHeader);

    for (int i = 0; i < _talkTable->getStringCount(); ++i) {
        writer.putStruct(strData[i]);
    }
    for (int i = 0; i < _talkTable->getStringCount(); ++i) {
        writer.putCString(_talkTable->getString(i).text);
    }
}

} // namespace resource

} // namespace reone
