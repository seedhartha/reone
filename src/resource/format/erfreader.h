/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "../resourceprovider.h"
#include "../types.h"

#include "binreader.h"

namespace reone {

namespace resource {

class ErfReader : public BinaryResourceReader, public IResourceProvider {
public:
    struct KeyEntry {
        ResourceId resId;
    };

    struct ResourceEntry {
        uint32_t offset {0};
        uint32_t size {0};
    };

    ErfReader(int id = kDefaultProviderId);

    std::shared_ptr<ByteArray> find(const ResourceId &id) override;

    int entryCount() const { return _entryCount; }
    const std::vector<KeyEntry> &keys() const { return _keys; }

    int getId() const override { return _id; }
    ByteArray getResourceData(int idx);

private:
    int _id;

    int _entryCount {0};
    uint32_t _keysOffset {0};
    uint32_t _resourcesOffset {0};
    std::vector<KeyEntry> _keys;
    std::vector<ResourceEntry> _resources;
    std::unordered_map<ResourceId, int, ResourceIdHasher> _resIdxByResId;

    void doLoad() override;

    void checkSignature();
    void loadKeys();
    void loadResources();

    KeyEntry readKeyEntry();
    ResourceEntry readResourceEntry();
    ByteArray getResourceData(const ResourceEntry &res);
};

} // namespace resource

} // namespace reone
