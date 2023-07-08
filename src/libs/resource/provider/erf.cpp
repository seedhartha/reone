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

#include "reone/system/stream/fileinput.h"

#include "reone/resource/format/erfreader.h"

namespace reone {

namespace resource {

void ErfResourceProvider::init() {
    auto erf = FileInputStream(_path, OpenMode::Binary);

    auto reader = ErfReader(erf);
    reader.load();

    auto &keys = reader.keys();
    auto &erfResources = reader.resources();

    for (auto i = 0; i < keys.size(); ++i) {
        auto resource = Resource();
        resource.id = keys[i].resId;
        resource.offset = erfResources[i].offset;
        resource.fileSize = erfResources[i].size;
        _resources[keys[i].resId] = std::move(resource);
    }
}

std::shared_ptr<ByteArray> ErfResourceProvider::find(const ResourceId &id) {
    auto maybeResource = _resources.find(id);
    if (maybeResource == _resources.end()) {
        return nullptr;
    }
    auto &resource = maybeResource->second;

    auto buffer = std::make_shared<ByteArray>(resource.fileSize, '\0');
    auto erf = FileInputStream(_path, OpenMode::Binary);
    erf.seek(resource.offset, SeekOrigin::Begin);
    erf.read(buffer->data(), buffer->size());
    return std::move(buffer);
}

} // namespace resource

} // namespace reone
