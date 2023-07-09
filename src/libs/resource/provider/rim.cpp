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

#include "reone/resource/provider/rim.h"

#include "reone/resource/format/rimreader.h"
#include "reone/system/stream/fileinput.h"

namespace reone {

namespace resource {

void RimResourceProvider::init() {
    _rim = std::make_unique<FileInputStream>(_path);

    auto reader = RimReader(*_rim);
    reader.load();

    for (auto &rimResource : reader.resources()) {
        auto resource = Resource();
        resource.id = rimResource.resId;
        resource.offset = rimResource.offset;
        resource.fileSize = rimResource.size;
        _resourceIds.insert(resource.id);
        _idToResource.insert(std::make_pair(resource.id, std::move(resource)));
    }
}

std::shared_ptr<ByteBuffer> RimResourceProvider::findResourceData(const ResourceId &id) {
    auto it = _idToResource.find(id);
    if (it == _idToResource.end()) {
        return nullptr;
    }
    auto &resource = it->second;
    if (resource.fileSize == 0) {
        return std::make_shared<ByteBuffer>();
    }

    _rim->seek(resource.offset, SeekOrigin::Begin);
    auto buf = std::make_shared<ByteBuffer>();
    buf->resize(resource.fileSize);
    _rim->read(&(*buf)[0], buf->size());

    return buf;
}

} // namespace resource

} // namespace reone
