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

#include "keyfile.h"

#include <boost/algorithm/string.hpp>

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static constexpr int kSignatureSize = 8;
static const char kSignature[] = "KEY V1  ";

KeyFile::KeyFile() : BinaryFile(kSignatureSize, kSignature) {
}

void KeyFile::doLoad() {
    _bifCount = readUint32();
    _keyCount = readUint32();
    _filesOffset = readUint32();
    _keysOffset = readUint32();

    loadFiles();
    loadKeys();
}

void KeyFile::loadFiles() {
    _files.reserve(_bifCount);
    seek(_filesOffset);

    for (int i = 0; i < _bifCount; ++i) {
        _files.push_back(readFileEntry());
    }
}

KeyFile::FileEntry KeyFile::readFileEntry() {
    uint32_t fileSize = readUint32();
    uint32_t filenameOffset = readUint32();
    uint16_t filenameSize = readUint16();
    ignore(2);

    FileEntry entry;
    entry.fileSize = fileSize;
    entry.filename = readCString(filenameOffset, filenameSize);

    return move(entry);
}

void KeyFile::loadKeys() {
    _keys.reserve(_keyCount);
    seek(_keysOffset);

    for (int i = 0; i < _keyCount; ++i) {
        _keys.push_back(readKeyEntry());
    }
}

KeyFile::KeyEntry KeyFile::readKeyEntry() {
    string resRef(readCString(16));
    uint16_t resType = readUint16();
    uint32_t resId = readUint32();

    KeyEntry entry;
    entry.resRef = boost::to_lower_copy(resRef);
    entry.resType = static_cast<ResourceType>(resType);
    entry.bifIdx = resId >> 20;
    entry.resIdx = resId & 0xfffff;

    return entry;
}

const string &KeyFile::getFilename(int idx) const {
    if (idx >= _files.size()) {
        throw out_of_range("KEY: file index out of range: " + to_string(idx));
    }
    return _files[idx].filename;
}

bool KeyFile::find(const string &resRef, ResourceType type, KeyEntry &key) const {
    string lcResRef(boost::to_lower_copy(resRef));

    auto it = find_if(
        _keys.begin(),
        _keys.end(),
        [&](const KeyEntry &e) { return e.resRef == lcResRef && e.resType == type; });

    if (it == _keys.end()) return false;

    key = *it;

    return true;
}

} // namespace resource

} // namespace reone
