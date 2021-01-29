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

#include "../types.h"

#include "binfile.h"

namespace reone {

namespace resource {

class KeyFile : public BinaryFile {
public:
    struct FileEntry {
        uint32_t fileSize { 0 };
        std::string filename;
    };

    struct KeyEntry {
        std::string resRef;
        ResourceType resType { ResourceType::Invalid };
        int bifIdx { 0 };
        int resIdx { 0 };
    };

    KeyFile();

    const std::string &getFilename(int idx) const;
    bool find(const std::string &resRef, ResourceType type, KeyEntry &key) const;

    const std::vector<FileEntry> &files() const { return _files; }
    const std::vector<KeyEntry> &keys() const { return _keys; }

private:
    int _bifCount { 0 };
    int _keyCount { 0 };
    uint32_t _filesOffset { 0 };
    uint32_t _keysOffset { 0 };
    std::vector<FileEntry> _files;
    std::vector<KeyEntry> _keys;

    void doLoad() override;
    void loadFiles();
    FileEntry readFileEntry();
    void loadKeys();
    KeyEntry readKeyEntry();
};

} // namespace resource

} // namespace reone
