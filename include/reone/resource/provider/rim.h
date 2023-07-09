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

#include "../provider.h"

namespace reone {

namespace resource {

class RimResourceProvider : public IResourceProvider {
public:
    struct Resource {
        ResourceId id;
        uint32_t offset {0};
        uint32_t fileSize {0};
    };

    RimResourceProvider(boost::filesystem::path path, int id = kDefaultProviderId) :
        _path(std::move(path)),
        _id(id) {
    }

    void init();

    const std::unordered_map<ResourceId, Resource, ResourceIdHasher> &resources() const { return _resources; }

    // IResourceProvider

    std::shared_ptr<ByteBuffer> find(const ResourceId &id) override;

    int id() const override { return _id; }

    // END IResourceProvider

private:
    boost::filesystem::path _path;
    int _id;

    std::unordered_map<ResourceId, Resource, ResourceIdHasher> _resources;
};

} // namespace resource

} // namespace reone
