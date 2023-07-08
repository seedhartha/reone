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

#pragma once

#include "reone/system/binaryreader.h"
#include "reone/system/stream/input.h"

#include "../id.h"
#include "../types.h"

namespace reone {

namespace resource {

class KeyReader : boost::noncopyable {
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

    KeyReader(IInputStream &key) :
        _key(BinaryReader(key)) {
    }

    void load();

    const std::vector<FileEntry> &files() const { return _files; }
    const std::vector<KeyEntry> &keys() const { return _keys; }

private:
    BinaryReader _key;

    uint32_t _numBifs {0};
    uint32_t _numKeys {0};
    uint32_t _offFiles {0};
    uint32_t _offKeys {0};

    std::vector<FileEntry> _files;
    std::vector<KeyEntry> _keys;

    void loadFiles();
    void loadKeys();

    FileEntry readFileEntry();
    KeyEntry readKeyEntry();
};

} // namespace resource

} // namespace reone
