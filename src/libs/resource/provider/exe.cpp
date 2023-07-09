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

#include "reone/resource/provider/exe.h"

#include "reone/resource/format/pereader.h"
#include "reone/system/stream/fileinput.h"

namespace reone {

namespace resource {

static std::unordered_map<PEResourceType, ResourceType> kPEResTypeToResType {
    {PEResourceType::Cursor, ResourceType::Cursor}, //
    {PEResourceType::CursorGroup, ResourceType::CursorGroup}};

void ExeResourceProvider::init() {
    _exe = std::make_unique<FileInputStream>(_path);

    auto reader = PeReader(*_exe);
    reader.load();

    for (auto &peRes : reader.resources()) {
        auto resType = kPEResTypeToResType.find(peRes.type);
        if (resType == kPEResTypeToResType.end()) {
            continue;
        }
        auto resId = ResourceId(std::to_string(peRes.name), resType->second);
        _resourceIds.insert(resId);
        Resource res;
        res.offset = peRes.offset;
        res.size = peRes.size;
        _idToResource.insert(std::make_pair(resId, std::move(res)));
    }
}

std::optional<ByteBuffer> ExeResourceProvider::findResourceData(const ResourceId &id) {
    auto it = _idToResource.find(id);
    if (it == _idToResource.end()) {
        return std::nullopt;
    }
    auto &res = it->second;
    if (res.size == 0) {
        return ByteBuffer();
    }
    ByteBuffer buf;
    buf.resize(res.size);

    _exe->seek(res.offset, SeekOrigin::Begin);
    _exe->read(&buf[0], buf.size());

    return buf;
}

} // namespace resource

} // namespace reone
