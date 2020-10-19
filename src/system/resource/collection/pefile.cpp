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

#include "pefile.h"

#include "../../log.h"

using namespace std;

namespace reone {

namespace resource {

static const int kNameMaskString = 0x80000000;
static const int kSiblingMaskDir = 0x80000000;

PEFile::PEFile() : BinaryFile(2, "MZ") {
}

shared_ptr<ByteArray> PEFile::find(uint32_t name, PEResourceType type) {
    auto resource = find_if(
        _resources.begin(),
        _resources.end(),
        [&name, &type](const Resource &res) { return res.type == type && res.name == name; });

    if (resource == _resources.end()) return nullptr;

    return getResourceData(*resource);
}

shared_ptr<ByteArray> PEFile::getResourceData(const Resource &res) {
    return make_shared<ByteArray>(readArray<char>(res.offset, res.size));
}

void PEFile::doLoad() {
    ignore(58);

    uint32_t offPeHeader = readUint32();
    seek(offPeHeader);

    uint32_t imageType = readUint32();

    loadHeader();
    loadOptionalHeader();

    for (int i = 0; i < _sectionCount; ++i) {
        loadSection();
    }

    auto resSection = find_if(_sections.begin(), _sections.end(), [](const Section &s) { return s.name == ".rsrc"; });
    seek(resSection->offset);
    loadResourceDir(*resSection, 0);
}

void PEFile::loadHeader() {
    ignore(2);

    _sectionCount = readUint16();
    _sections.reserve(_sectionCount);

    ignore(16);
}

void PEFile::loadOptionalHeader() {
    ignore(92);

    uint32_t dirCount = readUint32();

    ignore(dirCount * 8);
}

void PEFile::loadSection() {
    string name(readFixedString(8));

    ignore(4);

    uint32_t virtAddr = readUint32();
    uint32_t sizeRaw = readUint32();
    uint32_t offRaw = readUint32();

    ignore(16);

    _sections.push_back({ name, virtAddr, offRaw });
}

void PEFile::loadResourceDir(const Section &section, int level) {
    ignore(12);

    uint16_t namedEntryCount = readUint16();
    uint16_t idEntryCount = readUint16();
    int entryCount = namedEntryCount + idEntryCount;

    for (int i = 0; i < entryCount; ++i) {
        loadResourceDirEntry(section, level);
    }
}

void PEFile::loadResourceDirEntry(const Section &section, int level) {
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

    uint32_t pos = tell();
    uint32_t offSibling = section.offset + (sibling & 0x7fffffff);
    seek(offSibling);

    if (siblingDir) {
        loadResourceDir(section, level + 1);
    } else {
        loadResourceDataEntry(section);
    }

    seek(pos);
}

void PEFile::loadResourceDataEntry(const Section &section) {
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

} // namespace resources

} // namespace reone
