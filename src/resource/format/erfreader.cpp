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

#include "erfreader.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static constexpr int kSignatureSize = 8;
static const char kSignatureErf[] = "ERF V1.0";
static const char kSignatureMod[] = "MOD V1.0";

ErfReader::ErfReader(int id) :
    BinaryResourceReader(0, nullptr),
    _id(id) {
}

void ErfReader::doLoad() {
    checkSignature();
    ignore(8);

    _entryCount = readUint32();

    ignore(4);

    _keysOffset = readUint32();
    _resourcesOffset = readUint32();

    loadKeys();
    loadResources();
}

void ErfReader::checkSignature() {
    if (_size < kSignatureSize) {
        throw runtime_error("Invalid binary file size");
    }
    string sign(_reader->getString(kSignatureSize));

    bool erf = strncmp(&sign[0], kSignatureErf, kSignatureSize) == 0;
    if (!erf) {
        bool mod = strncmp(&sign[0], kSignatureMod, kSignatureSize) == 0;
        if (!mod) {
            throw runtime_error("Invalid ERF file signature");
        }
    }
}

void ErfReader::loadKeys() {
    _keys.reserve(_entryCount);
    seek(_keysOffset);

    for (int i = 0; i < _entryCount; ++i) {
        KeyEntry key(readKeyEntry());
        _resIdxByResId.insert(make_pair(key.resId, i));
        _keys.push_back(move(key));
    }
}

ErfReader::KeyEntry ErfReader::readKeyEntry() {
    string resRef(boost::to_lower_copy(readCString(16)));
    uint32_t resId = readUint32();
    uint16_t resType = readUint16();
    ignore(2);

    KeyEntry key;
    key.resId = ResourceId(move(resRef), static_cast<ResourceType>(resType));

    return move(key);
}

void ErfReader::loadResources() {
    _resources.reserve(_entryCount);
    seek(_resourcesOffset);

    for (int i = 0; i < _entryCount; ++i) {
        _resources.push_back(readResourceEntry());
    }
}

ErfReader::ResourceEntry ErfReader::readResourceEntry() {
    uint32_t offset = readUint32();
    uint32_t size = readUint32();

    ResourceEntry res;
    res.offset = offset;
    res.size = size;

    return move(res);
}

shared_ptr<ByteArray> ErfReader::find(const ResourceId &id) {
    auto maybeIdx = _resIdxByResId.find(id);
    if (maybeIdx == _resIdxByResId.end()) {
        return nullptr;
    }
    const ResourceEntry &res = _resources[maybeIdx->second];
    return make_shared<ByteArray>(getResourceData(res));
}

ByteArray ErfReader::getResourceData(const ResourceEntry &res) {
    return readBytes(res.offset, res.size);
}

ByteArray ErfReader::getResourceData(int idx) {
    if (idx >= _entryCount) {
        throw out_of_range("ERF: resource index out of range: " + to_string(idx));
    }
    return getResourceData(_resources[idx]);
}

} // namespace resource

} // namespace reone
