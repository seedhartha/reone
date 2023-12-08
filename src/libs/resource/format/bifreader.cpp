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

#include "reone/resource/format/bifreader.h"

#include "reone/system/checkutil.h"

namespace reone {

namespace resource {

void BifReader::load() {
    checkEqual("BIF signature", _bif.readString(8), std::string("BIFFV1  ", 8));
    loadHeader();
    loadResources();
}

void BifReader::loadHeader() {
    auto numVariableResources = _bif.readUint32();
    auto numFixedResources = _bif.readUint32();
    auto offVariableTable = _bif.readUint32();

    _numResources = numVariableResources;
    _offResources = offVariableTable;
}

void BifReader::loadResources() {
    _resources.reserve(_numResources);
    _bif.seek(_offResources);

    for (auto i = 0; i < _numResources; ++i) {
        _resources.push_back(readResourceEntry());
    }
}

BifReader::ResourceEntry BifReader::readResourceEntry() {
    uint32_t id = _bif.readUint32();
    uint32_t offset = _bif.readUint32();
    uint32_t fileSize = _bif.readUint32();
    uint32_t resType = _bif.readUint32();

    auto entry = ResourceEntry();
    entry.id = id;
    entry.offset = offset;
    entry.fileSize = fileSize;
    entry.resType = resType;

    return entry;
}

} // namespace resource

} // namespace reone
