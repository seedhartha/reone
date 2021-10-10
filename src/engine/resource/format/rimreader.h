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

class RimReader : public BinaryReader, public IResourceProvider {
public:
    struct Resource {
        std::string resRef;
        ResourceType resType {ResourceType::Invalid};
        uint32_t offset {0};
        uint32_t size {0};
    };

    RimReader(int id = kDefaultProviderId);

    std::shared_ptr<ByteArray> find(const std::string &resRef, ResourceType resType) override;

    const std::vector<Resource> &resources() const { return _resources; }

    int getId() const override { return _id; }
    ByteArray getResourceData(int idx);

private:
    int _id;

    int _resourceCount {0};
    uint32_t _resourcesOffset {0};
    std::vector<Resource> _resources;

    void doLoad() override;
    void loadResources();
    Resource readResource();
    ByteArray getResourceData(const Resource &res);
};

} // namespace resource

} // namespace reone
