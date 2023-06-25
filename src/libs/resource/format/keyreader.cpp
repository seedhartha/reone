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

namespace reone {

namespace resource {

void KeyReader::onLoad() {
    checkSignature(std::string("KEY V1  ", 8));

    _numBifs = readUint32();
    _numKeys = readUint32();
    _offFiles = readUint32();
    _offKeys = readUint32();

    loadFiles();
    loadKeys();
}

void KeyReader::loadFiles() {
    _files.reserve(_numBifs);

    seek(_offFiles);

    for (int i = 0; i < _numBifs; ++i) {
        _files.push_back(readFileEntry());
    }
}

KeyReader::FileEntry KeyReader::readFileEntry() {
    auto fileSize = readUint32();
    auto offFilename = readUint32();
    auto filenameSize = readUint16();
    auto drives = readUint16();

    auto entry = FileEntry();
    entry.filename = boost::replace_all_copy(readCString(offFilename, filenameSize), "\\", "/");
    entry.fileSize = fileSize;

    return std::move(entry);
}

void KeyReader::loadKeys() {
    _keys.reserve(_numKeys);
    seek(_offKeys);

    for (int i = 0; i < _numKeys; ++i) {
        _keys.push_back(readKeyEntry());
    }
}

KeyReader::KeyEntry KeyReader::readKeyEntry() {
    auto resRef = boost::to_lower_copy(readCString(16));
    auto resType = readUint16();
    auto resId = readUint32();

    auto entry = KeyEntry();
    entry.resId = ResourceId(std::move(resRef), static_cast<ResourceType>(resType));
    entry.bifIdx = resId >> 20;
    entry.resIdx = resId & 0xfffff;

    return std::move(entry);
}

} // namespace resource

} // namespace reone
