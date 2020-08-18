/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <string>
#include <vector>

#include "binfile.h"

namespace reone {

namespace resources {

enum class PEResourceType {
    Cursor = 1,
    Icon = 3,
    CursorGroup = 12,
    IconGroup = 14,
    Version = 16,
    Manifest = 24
};

class PEFile : public BinaryFile {
public:
    PEFile();

    std::shared_ptr<ByteArray> find(uint32_t name, PEResourceType type);

private:

    struct Section {
        std::string name;
        uint32_t virtualAddress { 0 };
        uint32_t offset { 0 };
    };

    struct Resource {
        PEResourceType type { PEResourceType::Cursor };
        uint32_t name { 0 };
        uint32_t langId { 0 };
        uint32_t offset { 0 };
        uint32_t size { 0 };
    };

    int _sectionCount { 0 };
    PEResourceType _currentType { PEResourceType::Cursor };
    uint32_t _currentName { 0 };
    uint32_t _currentLangId { 0 };
    std::vector<Section> _sections;
    std::vector<Resource> _resources;

    void doLoad() override;
    void loadHeader();
    void loadOptionalHeader();
    void loadSection();
    void loadResourceDir(const Section &section, int level = 0);
    void loadResourceDirEntry(const Section &section, int level = 0);
    void loadResourceDataEntry(const Section &section);
    std::shared_ptr<ByteArray> getResourceData(const Resource &res);
};

} // namespace resources

} // namespace reone
