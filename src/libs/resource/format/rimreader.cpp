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

#include "reone/resource/format/rimreader.h"

#include "reone/resource/format/signutil.h"

namespace reone {

namespace resource {

void RimReader::load() {
    checkSignature(_rim, std::string("RIM V1.0", 8));
    _rim.ignore(4);

    _numResources = _rim.readUint32();
    _offResources = _rim.readUint32();

    loadResources();
}

void RimReader::loadResources() {
    _resources.reserve(_numResources);
    _rim.seek(_offResources);

    for (int i = 0; i < _numResources; ++i) {
        _resources.push_back(readResource());
    }
}

RimReader::ResourceEntry RimReader::readResource() {
    auto resRef = boost::to_lower_copy(_rim.readCString(16));
    auto type = _rim.readUint16();
    _rim.ignore(4 + 2);
    auto offset = _rim.readUint32();
    auto size = _rim.readUint32();

    ResourceEntry resource;
    resource.resId = ResourceId(resRef, static_cast<ResourceType>(type));
    resource.offset = offset;
    resource.size = size;

    return std::move(resource);
}

} // namespace resource

} // namespace reone
