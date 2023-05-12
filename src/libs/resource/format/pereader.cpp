/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/system/logutil.h"

using namespace std;

namespace reone {

namespace resource {

static constexpr int kNameMaskString = 0x80000000;
static constexpr int kSiblingMaskDir = 0x80000000;

void PeReader::onLoad() {
    checkSignature(string("MZ", 2));
    ignore(58);

    uint32_t offPeHeader = readUint32();
    seek(offPeHeader);

    uint32_t imageType = readUint32();

    loadHeader();
    loadOptionalHeader();

    for (int i = 0; i < _sectionCount; ++i) {
        loadSection();
    }

    auto maybeSection = find_if(_sections.begin(), _sections.end(), [](auto &s) { return s.name == ".rsrc"; });
    if (maybeSection != _sections.end()) {
        seek(maybeSection->offset);
        loadResourceDir(*maybeSection, 0);
    }
}

shared_ptr<ByteArray> PeReader::find(uint32_t name, PEResourceType type) {
    return findInternal([&name, &type](const Resource &res) {
        return res.type == type && res.name == name;
    });
}

shared_ptr<ByteArray> PeReader::findInternal(function<bool(const Resource &)> pred) {
    auto maybeResource = find_if(_resources.begin(), _resources.end(), pred);
    if (maybeResource == _resources.end())
        return nullptr;

    return getResourceData(*maybeResource);
}

shared_ptr<ByteArray> PeReader::getResourceData(const Resource &res) {
    return make_shared<ByteArray>(readBytes(res.offset, res.size));
}

void PeReader::loadHeader() {
    ignore(2);

    _sectionCount = readUint16();
    _sections.reserve(_sectionCount);

    ignore(16);
}

void PeReader::loadOptionalHeader() {
    ignore(92);

    uint32_t dirCount = readUint32();

    ignore(dirCount * 8);
}

void PeReader::loadSection() {
    string name(readCString(8));

    ignore(4);

    uint32_t virtAddr = readUint32();
    uint32_t sizeRaw = readUint32();
    uint32_t offRaw = readUint32();

    ignore(16);

    _sections.push_back({name, virtAddr, offRaw});
}

void PeReader::loadResourceDir(const Section &section, int level) {
    ignore(12);

    uint16_t namedEntryCount = readUint16();
    uint16_t idEntryCount = readUint16();
    int entryCount = namedEntryCount + idEntryCount;

    for (int i = 0; i < entryCount; ++i) {
        loadResourceDirEntry(section, level);
    }
}

void PeReader::loadResourceDirEntry(const Section &section, int level) {
    uint32_t name = readUint32();

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

    uint32_t sibling = readUint32();
    bool siblingDir = (sibling & kSiblingMaskDir) != 0;

    size_t pos = tell();
    uint32_t offSibling = section.offset + (sibling & 0x7fffffff);
    seek(offSibling);

    if (siblingDir) {
        loadResourceDir(section, level + 1);
    } else {
        loadResourceDataEntry(section);
    }

    seek(pos);
}

void PeReader::loadResourceDataEntry(const Section &section) {
    uint32_t offData = readUint32();
    uint32_t sizeData = readUint32();

    ignore(8);

    Resource res;
    res.type = _currentType;
    res.name = _currentName;
    res.langId = _currentLangId;
    res.offset = section.offset + offData - section.virtualAddress;
    res.size = sizeData;

    _resources.push_back(move(res));
}

} // namespace resource

} // namespace reone
