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

#include "reone/resource/provider/folder.h"

#include "reone/resource/typeutil.h"
#include "reone/system/stream/fileinput.h"

namespace reone {

namespace resource {

void Folder::init() {
    loadDirectory(_path);
}

void Folder::loadDirectory(const std::filesystem::path &path) {
    for (auto &entry : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(entry.path())) {
            loadDirectory(entry.path());
            continue;
        }
        auto resRef = boost::to_lower_copy(entry.path().filename().replace_extension("").string());
        auto ext = boost::to_lower_copy(entry.path().extension().string().substr(1));
        auto resType = getResTypeByExt(ext);
        auto resId = ResourceId(resRef, resType);

        Resource res;
        res.path = entry.path();
        res.type = resType;

        _resourceIds.insert(resId);
        _idToResource.insert(std::make_pair(resId, std::move(res)));
    }
}

std::shared_ptr<ByteBuffer> Folder::findResourceData(const ResourceId &id) {
    auto it = _idToResource.find(id);
    if (it == _idToResource.end()) {
        return nullptr;
    }
    auto &resource = it->second;

    auto stream = FileInputStream(resource.path);
    auto len = stream.length();
    if (len == 0) {
        return std::make_shared<ByteBuffer>();
    }

    auto buf = std::make_shared<ByteBuffer>();
    buf->resize(len);
    stream.read(&(*buf)[0], buf->size());

    return buf;
}

} // namespace resource

} // namespace reone
