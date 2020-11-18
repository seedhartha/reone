/*
 * Copyright (c) 2020 The reone project contributors
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

#include "rimfile.h"

#include <boost/algorithm/string.hpp>

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static const int kSignatureSize = 8;
static const char kSignature[] = "RIM V1.0";

RimFile::RimFile() : BinaryFile(kSignatureSize, kSignature) {
}

void RimFile::doLoad() {
    ignore(4);

    _resourceCount = readUint32();
    _resourcesOffset = readUint32();

    loadResources();
}

void RimFile::loadResources() {
    _resources.reserve(_resourceCount);
    seek(_resourcesOffset);

    for (int i = 0; i < _resourceCount; ++i) {
        _resources.push_back(readResource());
    }
}

RimFile::Resource RimFile::readResource() {
    string resRef(readCString(16));
    uint16_t type = readUint16();
    ignore(4 + 2);
    uint32_t offset = readUint32();
    uint32_t size = readUint32();

    Resource res;
    res.resRef = boost::to_lower_copy(resRef);
    res.type = static_cast<ResourceType>(type);
    res.offset = offset;
    res.size = size;

    return move(res);
}

bool RimFile::supports(ResourceType type) const {
    return true;
}

shared_ptr<ByteArray> RimFile::find(const string &resRef, ResourceType type) {
    string lcResRef(boost::to_lower_copy(resRef));

    auto it = find_if(
        _resources.begin(),
        _resources.end(),
        [&](const Resource &res) { return res.resRef == lcResRef && res.type == type; });

    if (it == _resources.end()) return nullptr;

    return make_shared<ByteArray>(getResourceData(*it));
}

ByteArray RimFile::getResourceData(const Resource &res) {
    return readArray<char>(res.offset, res.size);
}

ByteArray RimFile::getResourceData(int idx) {
    if (idx >= _resourceCount) {
        throw logic_error("RIM: resource index out of range: " + to_string(idx));
    }
    return getResourceData(_resources[idx]);
}

const vector<RimFile::Resource> &RimFile::resources() const {
    return _resources;
}

} // namespace resource

} // namespace reone
