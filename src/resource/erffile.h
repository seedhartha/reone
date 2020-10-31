/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "binfile.h"
#include "types.h"

namespace reone {

namespace resource {

class ErfFile : public BinaryFile, public IResourceProvider {
public:
    struct Key {
        std::string resRef { 0 };
        uint32_t resId { 0 };
        ResourceType resType { ResourceType::Invalid };
    };

    struct Resource {
        uint32_t offset { 0 };
        uint32_t size { 0 };
    };

    ErfFile();

    bool supports(ResourceType type) const override;
    std::shared_ptr<ByteArray> find(const std::string &resRef, ResourceType type) override;
    ByteArray getResourceData(int idx);

    int entryCount() const;
    const std::vector<Key> &keys() const;

private:
    int _entryCount { 0 };
    uint32_t _keysOffset { 0 };
    uint32_t _resourcesOffset { 0 };
    std::vector<Key> _keys;
    std::vector<Resource> _resources;

    void doLoad() override;

    void checkSignature();
    void loadKeys();
    Key readKey();
    void loadResources();
    Resource readResource();
    ByteArray getResourceData(const Resource &res);
};

} // namespace resource

} // namespace reone
