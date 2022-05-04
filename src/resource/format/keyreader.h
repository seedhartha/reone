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

#pragma once

#include "../id.h"
#include "../types.h"

#include "binreader.h"

namespace reone {

namespace resource {

class KeyReader : public BinaryResourceReader {
public:
    struct FileEntry {
        std::string filename;
        uint32_t fileSize {0};
    };

    struct KeyEntry {
        ResourceId resId;
        int bifIdx {0};
        int resIdx {0};
    };

    bool find(const ResourceId &id, KeyEntry &outKey) const;
    const std::string &getFilename(int idx) const;

    const std::vector<FileEntry> &files() const { return _files; }
    const std::vector<KeyEntry> &keys() const { return _keys; }

private:
    int _bifCount {0};
    int _keyCount {0};
    uint32_t _filesOffset {0};
    uint32_t _keysOffset {0};
    std::vector<FileEntry> _files;
    std::vector<KeyEntry> _keys;
    std::unordered_map<ResourceId, int, ResourceIdHasher> _keyIdxByResId;

    void onLoad() override;

    void loadFiles();
    void loadKeys();

    FileEntry readFileEntry();
    KeyEntry readKeyEntry();
};

} // namespace resource

} // namespace reone
