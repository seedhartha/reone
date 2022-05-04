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

#include "rimreader.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static constexpr int kSignatureSize = 8;
static const char kSignature[] = "RIM V1.0";

RimReader::RimReader(int id) :
    BinaryResourceReader(kSignatureSize, kSignature),
    _id(id) {
}

void RimReader::doLoad() {
    ignore(4);

    _resourceCount = readUint32();
    _resourcesOffset = readUint32();

    loadResources();
}

void RimReader::loadResources() {
    _resources.reserve(_resourceCount);
    seek(_resourcesOffset);

    for (int i = 0; i < _resourceCount; ++i) {
        ResourceEntry res(readResource());
        _resIdxByResId.insert(make_pair(res.resId, i));
        _resources.push_back(move(res));
    }
}

RimReader::ResourceEntry RimReader::readResource() {
    string resRef(boost::to_lower_copy(readCString(16)));
    uint16_t type = readUint16();
    ignore(4 + 2);
    uint32_t offset = readUint32();
    uint32_t size = readUint32();

    ResourceEntry res;
    res.resId = ResourceId(resRef, static_cast<ResourceType>(type));
    res.offset = offset;
    res.size = size;

    return move(res);
}

shared_ptr<ByteArray> RimReader::find(const ResourceId &id) {
    auto maybeIdx = _resIdxByResId.find(id);
    if (maybeIdx == _resIdxByResId.end()) {
        return nullptr;
    }
    const ResourceEntry &res = _resources[maybeIdx->second];
    return make_shared<ByteArray>(getResourceData(res));
}

ByteArray RimReader::getResourceData(const ResourceEntry &res) {
    return readBytes(res.offset, res.size);
}

ByteArray RimReader::getResourceData(int idx) {
    if (idx >= _resourceCount) {
        throw logic_error("RIM: resource index out of range: " + to_string(idx));
    }
    return getResourceData(_resources[idx]);
}

} // namespace resource

} // namespace reone
