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

#include "rim.h"

#include "../../common/stream/fileinput.h"

#include "../format/rimreader.h"

using namespace std;

namespace reone {

namespace resource {

void RimResourceProvider::init() {
    auto rim = FileInputStream(_path, OpenMode::Binary);

    auto reader = RimReader();
    reader.load(rim);

    for (auto &rimResource : reader.resources()) {
        auto resource = Resource();
        resource.id = rimResource.resId;
        resource.offset = rimResource.offset;
        resource.fileSize = rimResource.size;
        _resources[rimResource.resId] = move(resource);
    }
}

shared_ptr<ByteArray> RimResourceProvider::find(const ResourceId &id) {
    auto maybeResource = _resources.find(id);
    if (maybeResource == _resources.end()) {
        return nullptr;
    }
    auto &resource = maybeResource->second;

    auto buffer = make_shared<ByteArray>(resource.fileSize, '\0');
    auto rim = FileInputStream(_path, OpenMode::Binary);
    rim.seek(resource.offset, SeekOrigin::Begin);
    rim.read(buffer->data(), buffer->size());
    return move(buffer);
}

} // namespace resource

} // namespace reone
