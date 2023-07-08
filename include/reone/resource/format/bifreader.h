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

namespace reone {

namespace resource {

class BifReader : boost::noncopyable {
public:
    struct ResourceEntry {
        uint32_t id {0};
        uint32_t offset {0};
        uint32_t fileSize {0};
        uint32_t resType {0};
    };

    BifReader(IInputStream &bif) :
        _bif(BinaryReader(bif)) {
    }

    void load();

    const std::vector<ResourceEntry> &resources() const {
        return _resources;
    }

private:
    BinaryReader _bif;

    int _numResources {0};
    uint32_t _offResources {0};

    std::vector<ResourceEntry> _resources;

    void loadHeader();
    void loadResources();

    ResourceEntry readResourceEntry();
};

} // namespace resource

} // namespace reone
