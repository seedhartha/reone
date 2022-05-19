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

#include "bifreader.h"

using namespace std;

namespace reone {

namespace resource {

void BifReader::onLoad() {
    checkSignature(string("BIFFV1  ", 8));
    loadHeader();
    loadResources();
}

void BifReader::loadHeader() {
    auto numVariableResources = readUint32();
    auto numFixedResources = readUint32();
    auto offVariableTable = readUint32();

    _numResources = numVariableResources;
    _offResources = offVariableTable;
}

void BifReader::loadResources() {
    _resources.reserve(_numResources);
    seek(_offResources);

    for (auto i = 0; i < _numResources; ++i) {
        _resources.push_back(readResourceEntry());
    }
}

BifReader::ResourceEntry BifReader::readResourceEntry() {
    uint32_t id = readUint32();
    uint32_t offset = readUint32();
    uint32_t fileSize = readUint32();
    uint32_t resType = readUint32();

    auto entry = ResourceEntry();
    entry.id = id;
    entry.offset = offset;
    entry.fileSize = fileSize;
    entry.resType = resType;

    return move(entry);
}

} // namespace resource

} // namespace reone
