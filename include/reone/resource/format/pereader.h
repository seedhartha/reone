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
#include "reone/system/types.h"

#include "../types.h"

namespace reone {

namespace resource {

class PeReader : boost::noncopyable {
public:
    PeReader(IInputStream &pe) :
        _pe(BinaryReader(pe)) {
    }

    void load();

    std::shared_ptr<ByteArray> find(uint32_t name, PEResourceType type);

private:
    struct Section {
        std::string name;
        uint32_t virtualAddress {0};
        uint32_t offset {0};
    };

    struct Resource {
        PEResourceType type {PEResourceType::Cursor};
        uint32_t name {0};
        uint32_t langId {0};
        uint32_t offset {0};
        uint32_t size {0};
    };

    BinaryReader _pe;

    int _sectionCount {0};
    PEResourceType _currentType {PEResourceType::Cursor};
    uint32_t _currentName {0};
    uint32_t _currentLangId {0};
    std::vector<Section> _sections;
    std::vector<Resource> _resources;

    void loadHeader();
    void loadOptionalHeader();
    void loadSection();
    void loadResourceDir(const Section &section, int level = 0);
    void loadResourceDirEntry(const Section &section, int level = 0);
    void loadResourceDataEntry(const Section &section);

    std::shared_ptr<ByteArray> findInternal(std::function<bool(const Resource &)> pred);

    std::shared_ptr<ByteArray> getResourceData(const Resource &res);
};

} // namespace resource

} // namespace reone
