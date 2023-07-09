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

#include "reone/system/collectionutil.h"
#include "reone/system/fileutil.h"
#include "reone/system/stream/fileinput.h"

#include "reone/resource/format/bifreader.h"
#include "reone/resource/format/keyreader.h"

namespace reone {

namespace resource {

void KeyBifResourceProvider::init() {
    auto key = FileInputStream(_keyPath);
    auto keyReader = KeyReader(key);
    keyReader.load();

    auto gamePath = _keyPath.parent_path();
    auto &files = keyReader.files();

    auto keysByBifIdx = groupBy<KeyReader::KeyEntry, int, const KeyReader::KeyEntry *>(
        keyReader.keys(),
        [](auto &item) { return item.bifIdx; },
        [](auto &item) { return &item; });

    for (auto i = 0; i < keyReader.files().size(); ++i) {
        auto &file = keyReader.files()[i];
        auto bifPath = findFileIgnoreCase(gamePath, file.filename);
        _bifPaths.push_back(bifPath);

        auto bif = FileInputStream(bifPath);
        auto bifReader = BifReader(bif);
        bifReader.load();

        auto &keys = keysByBifIdx.at(i);
        auto &bifResources = bifReader.resources();

        for (auto &key : keys) {
            auto &bifResource = bifResources[key->resIdx];

            auto resource = Resource();
            resource.bifIdx = key->bifIdx;
            resource.bifOffset = bifResource.offset;
            resource.fileSize = bifResource.fileSize;

            _resources[key->resId] = std::move(resource);
        }
    }
}

std::shared_ptr<ByteBuffer> KeyBifResourceProvider::find(const ResourceId &id) {
    auto maybeResource = _resources.find(id);
    if (maybeResource == _resources.end()) {
        return nullptr;
    }
    auto &resource = maybeResource->second;

    auto buffer = std::make_shared<ByteBuffer>(resource.fileSize, '\0');

    auto &bifPath = _bifPaths.at(resource.bifIdx);
    auto bif = FileInputStream(bifPath);
    bif.seek(resource.bifOffset, SeekOrigin::Begin);
    bif.read(buffer->data(), resource.fileSize);

    return std::move(buffer);
}

} // namespace resource

} // namespace reone
