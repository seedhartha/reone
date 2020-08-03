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

#include "erffile.h"

#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

static const int kSignatureSize = 8;
static const char kSignature[] = "ERF V1.0";

ErfFile::ErfFile() : BinaryFile(kSignatureSize, kSignature) {
}

void ErfFile::doLoad() {
    ignore(8);

    _entryCount = readUint32();

    ignore(4);

    _keysOffset = readUint32();
    _resourcesOffset = readUint32();

    loadKeys();
    loadResources();
}

void ErfFile::loadKeys() {
    _keys.reserve(_entryCount);
    seek(_keysOffset);

    for (int i = 0; i < _entryCount; ++i) {
        _keys.push_back(readKey());
    }
}

ErfFile::Key ErfFile::readKey() {
    std::string resRef(readFixedString(16));
    uint32_t resId = readUint32();
    uint16_t resType = readUint16();
    ignore(2);

    Key key;
    key.resRef = boost::to_lower_copy(resRef);
    key.resId = resId;
    key.resType = static_cast<ResourceType>(resType);

    return std::move(key);
}

void ErfFile::loadResources() {
    _resources.reserve(_entryCount);
    seek(_resourcesOffset);

    for (int i = 0; i < _entryCount; ++i) {
        _resources.push_back(readResource());
    }
}

ErfFile::Resource ErfFile::readResource() {
    uint32_t offset = readUint32();
    uint32_t size = readUint32();

    Resource res;
    res.offset = offset;
    res.size = size;

    return std::move(res);
}

bool ErfFile::supports(ResourceType type) const {
    return true;
}

std::shared_ptr<ByteArray> ErfFile::find(const std::string &resRef, ResourceType type) {
    std::string lcResRef(boost::to_lower_copy(resRef));
    int idx = -1;

    for (int i = 0; i < _entryCount; ++i) {
        if (_keys[i].resRef == lcResRef && _keys[i].resType == type) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return nullptr;
    const Resource &res = _resources[idx];

    return std::make_shared<ByteArray>(getResourceData(res));
}

ByteArray ErfFile::getResourceData(const Resource &res) {
    return readArray<char>(res.offset, res.size);
}

ByteArray ErfFile::getResourceData(int idx) {
    if (idx >= _entryCount) {
        throw std::out_of_range("ERF: resource index out of range: " + std::to_string(idx));
    }
    return getResourceData(_resources[idx]);
}

int ErfFile::entryCount() const {
    return _entryCount;
}

const std::vector<ErfFile::Key> &ErfFile::keys() const {
    return _keys;
}

} // namespace resources

} // namespace reone
