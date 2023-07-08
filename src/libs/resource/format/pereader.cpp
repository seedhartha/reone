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
    _pe.ignore(58);

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

std::shared_ptr<ByteArray> PeReader::find(uint32_t name, PEResourceType type) {
    return findInternal([&name, &type](const Resource &res) {
        return res.type == type && res.name == name;
    });
}

std::shared_ptr<ByteArray> PeReader::findInternal(std::function<bool(const Resource &)> pred) {
    auto maybeResource = std::find_if(_resources.begin(), _resources.end(), pred);
    if (maybeResource == _resources.end())
        return nullptr;

    return getResourceData(*maybeResource);
}

std::shared_ptr<ByteArray> PeReader::getResourceData(const Resource &res) {
    return std::make_shared<ByteArray>(_pe.readBytesAt(res.offset, res.size));
}

void PeReader::loadHeader() {
    _pe.ignore(2);

    _sectionCount = _pe.readUint16();
    _sections.reserve(_sectionCount);

    _pe.ignore(16);
}

void PeReader::loadOptionalHeader() {
    _pe.ignore(92);

    uint32_t dirCount = _pe.readUint32();

    _pe.ignore(dirCount * 8);
}

void PeReader::loadSection() {
    std::string name(_pe.readCString(8));

    _pe.ignore(4);

    uint32_t virtAddr = _pe.readUint32();
    uint32_t sizeRaw = _pe.readUint32();
    uint32_t offRaw = _pe.readUint32();

    _pe.ignore(16);

    _sections.push_back({name, virtAddr, offRaw});
}

void PeReader::loadResourceDir(const Section &section, int level) {
    _pe.ignore(12);

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

    _pe.ignore(8);

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
