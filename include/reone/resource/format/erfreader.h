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

#include "../id.h"

namespace reone {

namespace resource {

class ErfReader : boost::noncopyable {
public:
    struct KeyEntry {
        ResourceId resId;
    };

    struct ResourceEntry {
        uint32_t offset {0};
        uint32_t size {0};
    };

    ErfReader(IInputStream &erf) :
        _erf(BinaryReader(erf)) {
    }

    void load();

    const std::vector<KeyEntry> &keys() const { return _keys; }
    const std::vector<ResourceEntry> &resources() const { return _resources; }

private:
    BinaryReader _erf;

    int _numEntries {0};
    uint32_t _offKeys {0};
    uint32_t _offResources {0};

    std::vector<KeyEntry> _keys;
    std::vector<ResourceEntry> _resources;

    void checkSignature();
    void loadKeys();
    void loadResources();

    KeyEntry readKeyEntry();
    ResourceEntry readResourceEntry();
};

} // namespace resource

} // namespace reone
