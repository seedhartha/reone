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

#include "reone/resource/format/pereader.h"

#include "reone/resource/format/signutil.h"
#include "reone/system/logutil.h"

namespace reone {

namespace resource {

static constexpr int kNameMaskString = 0x80000000;
static constexpr int kSiblingMaskDir = 0x80000000;

void PeReader::load() {
    checkSignature(_pe, std::string("MZ", 2));
    _pe.skipBytes(58);

    uint32_t offPeHeader = _pe.readUint32();
    _pe.seek(offPeHeader);

    uint32_t imageType = _pe.readUint32();

    loadHeader();
    loadOptionalHeader();

    for (int i = 0; i < _sectionCount; ++i) {
        loadSection();
    }

    auto maybeSection = std::find_if(_sections.begin(), _sections.end(), [](auto &s) { return s.name == ".rsrc"; });
    if (maybeSection != _sections.end()) {
        _pe.seek(maybeSection->offset);
        loadResourceDir(*maybeSection, 0);
    }
}

void PeReader::loadHeader() {
    _pe.skipBytes(2);

    _sectionCount = _pe.readUint16();
    _sections.reserve(_sectionCount);

    _pe.skipBytes(16);
}

void PeReader::loadOptionalHeader() {
    _pe.skipBytes(92);

    uint32_t dirCount = _pe.readUint32();

    _pe.skipBytes(dirCount * 8);
}

void PeReader::loadSection() {
    std::string name(_pe.readString(8));

    _pe.skipBytes(4);

    uint32_t virtAddr = _pe.readUint32();
    uint32_t sizeRaw = _pe.readUint32();
    uint32_t offRaw = _pe.readUint32();

    _pe.skipBytes(16);

    _sections.push_back({name, virtAddr, offRaw});
}

void PeReader::loadResourceDir(const Section &section, int level) {
    _pe.skipBytes(12);

    uint16_t namedEntryCount = _pe.readUint16();
    uint16_t idEntryCount = _pe.readUint16();
    int entryCount = namedEntryCount + idEntryCount;

    for (int i = 0; i < entryCount; ++i) {
        loadResourceDirEntry(section, level);
    }
}

void PeReader::loadResourceDirEntry(const Section &section, int level) {
    uint32_t name = _pe.readUint32();

    switch (level) {
    case 0:
        _currentType = static_cast<PEResourceType>(name);
        break;
    case 1:
        _currentName = name;
        break;
    case 2:
        _currentLangId = name;
        break;
    default:
        break;
    }

    uint32_t sibling = _pe.readUint32();
    bool siblingDir = (sibling & kSiblingMaskDir) != 0;

    size_t pos = _pe.position();
    uint32_t offSibling = section.offset + (sibling & 0x7fffffff);
    _pe.seek(offSibling);

    if (siblingDir) {
        loadResourceDir(section, level + 1);
    } else {
        loadResourceDataEntry(section);
    }

    _pe.seek(pos);
}

void PeReader::loadResourceDataEntry(const Section &section) {
    uint32_t offData = _pe.readUint32();
    uint32_t sizeData = _pe.readUint32();

    _pe.skipBytes(8);

    Resource res;
    res.type = _currentType;
    res.name = _currentName;
    res.langId = _currentLangId;
    res.offset = section.offset + offData - section.virtualAddress;
    res.size = sizeData;

    _resources.push_back(std::move(res));
}

} // namespace resource

} // namespace reone
