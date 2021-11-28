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

#include "keyreader.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static constexpr int kSignatureSize = 8;
static const char kSignature[] = "KEY V1  ";

KeyReader::KeyReader() :
    BinaryReader(kSignatureSize, kSignature) {
}

bool KeyReader::find(const ResourceId &id, KeyEntry &outKey) const {
    auto maybeKey = _keyIdxByResId.find(id);
    if (maybeKey == _keyIdxByResId.end()) {
        return false;
    }
    outKey = _keys[maybeKey->second];
    return true;
}

const string &KeyReader::getFilename(int idx) const {
    if (idx >= _files.size()) {
        throw out_of_range("KEY: file index out of range: " + to_string(idx));
    }
    return _files[idx].filename;
}

void KeyReader::doLoad() {
    _bifCount = readUint32();
    _keyCount = readUint32();
    _filesOffset = readUint32();
    _keysOffset = readUint32();

    loadFiles();
    loadKeys();
}

void KeyReader::loadFiles() {
    _files.reserve(_bifCount);
    seek(_filesOffset);

    for (int i = 0; i < _bifCount; ++i) {
        _files.push_back(readFileEntry());
    }
}

KeyReader::FileEntry KeyReader::readFileEntry() {
    uint32_t fileSize = readUint32();
    uint32_t filenameOffset = readUint32();
    uint16_t filenameSize = readUint16();
    ignore(2);

    FileEntry entry;
    entry.filename = readCString(filenameOffset, filenameSize);
    entry.fileSize = fileSize;

    return move(entry);
}

void KeyReader::loadKeys() {
    _keys.reserve(_keyCount);
    seek(_keysOffset);

    for (int i = 0; i < _keyCount; ++i) {
        KeyEntry entry(readKeyEntry());
        _keyIdxByResId.insert(make_pair(entry.resId, i));
        _keys.push_back(move(entry));
    }
}

KeyReader::KeyEntry KeyReader::readKeyEntry() {
    string resRef(boost::to_lower_copy(readCString(16)));
    uint16_t resType = readUint16();
    uint32_t resId = readUint32();

    KeyEntry entry;
    entry.resId = ResourceId(std::move(resRef), static_cast<ResourceType>(resType));
    entry.bifIdx = resId >> 20;
    entry.resIdx = resId & 0xfffff;

    return move(entry);
}

} // namespace resource

} // namespace reone
