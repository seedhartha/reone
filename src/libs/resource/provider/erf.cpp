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

#include "reone/resource/provider/erf.h"

#include "reone/resource/format/erfreader.h"

namespace reone {

namespace resource {

void ErfResourceProvider::init() {
    _erf = std::make_unique<FileInputStream>(_path);

    auto reader = ErfReader(*_erf);
    reader.load();

    auto &keys = reader.keys();
    auto &erfResources = reader.resources();

    for (auto i = 0; i < keys.size(); ++i) {
        auto resource = Resource();
        resource.id = keys[i].resId;
        resource.offset = erfResources[i].offset;
        resource.fileSize = erfResources[i].size;
        _resourceIds.insert(resource.id);
        _idToResource.insert(std::make_pair(keys[i].resId, std::move(resource)));
    }
}

std::shared_ptr<ByteBuffer> ErfResourceProvider::findResourceData(const ResourceId &id) {
    auto it = _idToResource.find(id);
    if (it == _idToResource.end()) {
        return nullptr;
    }
    auto &resource = it->second;
    if (resource.fileSize == 0) {
        return std::make_shared<ByteBuffer>();
    }

    _erf->seek(resource.offset, SeekOrigin::Begin);
    auto buf = std::make_shared<ByteBuffer>();
    buf->resize(resource.fileSize);
    _erf->read(&(*buf)[0], buf->size());

    return buf;
}

} // namespace resource

} // namespace reone
