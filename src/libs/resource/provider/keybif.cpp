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

#include "reone/resource/provider/keybif.h"

#include "reone/resource/format/bifreader.h"
#include "reone/resource/format/keyreader.h"
#include "reone/system/fileutil.h"
#include "reone/system/stream/fileinput.h"

namespace reone {

namespace resource {

void KeyBifResourceProvider::init() {
    auto key = FileInputStream(_keyPath);
    auto keyReader = KeyReader(key);
    keyReader.load();

    auto gamePath = _keyPath.parent_path();
    auto &files = keyReader.files();

    std::unordered_map<int, std::vector<const KeyReader::KeyEntry *>> bifIdxToKey;
    for (auto &key : keyReader.keys()) {
        bifIdxToKey[key.bifIdx].push_back(&key);
    }

    for (auto i = 0; i < keyReader.files().size(); ++i) {
        auto &file = keyReader.files()[i];
        auto bifPath = findFileIgnoreCase(gamePath, file.filename);
        _bifPaths.push_back(bifPath);

        auto bif = FileInputStream(bifPath);
        auto bifReader = BifReader(bif);
        bifReader.load();

        auto &keys = bifIdxToKey.at(i);
        auto &bifResources = bifReader.resources();

        for (auto &key : keys) {
            auto &bifResource = bifResources[key->resIdx];

            auto resource = Resource();
            resource.bifIdx = key->bifIdx;
            resource.bifOffset = bifResource.offset;
            resource.fileSize = bifResource.fileSize;

            _resourceIds.insert(key->resId);
            _idToResource.insert(std::make_pair(key->resId, std::move(resource)));
        }
    }
}

std::shared_ptr<ByteBuffer> KeyBifResourceProvider::findResourceData(const ResourceId &id) {
    auto it = _idToResource.find(id);
    if (it == _idToResource.end()) {
        return nullptr;
    }
    auto &resource = it->second;
    if (resource.fileSize == 0) {
        return std::make_shared<ByteBuffer>();
    }

    auto &path = _bifPaths.at(resource.bifIdx);
    auto bif = FileInputStream(path);
    bif.seek(resource.bifOffset, SeekOrigin::Begin);
    auto buf = std::make_shared<ByteBuffer>();
    buf->resize(resource.fileSize);
    bif.read(&(*buf)[0], buf->size());

    return buf;
}

} // namespace resource

} // namespace reone
