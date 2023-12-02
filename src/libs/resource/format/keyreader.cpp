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

#include "reone/resource/format/keyreader.h"

#include "reone/system/checkutil.h"

namespace reone {

namespace resource {

void KeyReader::load() {
    checkEqual(_key.readString(8), std::string("KEY V1  ", 8), "Invalid KEY signature");

    _numBifs = _key.readUint32();
    _numKeys = _key.readUint32();
    _offFiles = _key.readUint32();
    _offKeys = _key.readUint32();

    loadFiles();
    loadKeys();
}

void KeyReader::loadFiles() {
    _files.reserve(_numBifs);

    _key.seek(_offFiles);

    for (int i = 0; i < _numBifs; ++i) {
        _files.push_back(readFileEntry());
    }
}

KeyReader::FileEntry KeyReader::readFileEntry() {
    auto fileSize = _key.readUint32();
    auto offFilename = _key.readUint32();
    auto filenameSize = _key.readUint16();
    auto drives = _key.readUint16();

    auto entry = FileEntry();
    entry.filename = boost::replace_all_copy(_key.readStringAt(offFilename, filenameSize), "\\", "/");
    entry.fileSize = fileSize;

    return entry;
}

void KeyReader::loadKeys() {
    _keys.reserve(_numKeys);
    _key.seek(_offKeys);

    for (int i = 0; i < _numKeys; ++i) {
        _keys.push_back(readKeyEntry());
    }
}

KeyReader::KeyEntry KeyReader::readKeyEntry() {
    auto resRef = boost::to_lower_copy(_key.readString(16));
    auto resType = _key.readUint16();
    auto resId = _key.readUint32();

    auto entry = KeyEntry();
    entry.resId = ResourceId(std::move(resRef), static_cast<ResType>(resType));
    entry.bifIdx = resId >> 20;
    entry.resIdx = resId & 0xfffff;

    return entry;
}

} // namespace resource

} // namespace reone
