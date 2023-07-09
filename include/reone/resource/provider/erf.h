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

#include "reone/system/stream/fileinput.h"

#include "../provider.h"

namespace reone {

namespace resource {

class ErfResourceProvider : public IResourceProvider, boost::noncopyable {
public:
    ErfResourceProvider(std::filesystem::path path) :
        _path(std::move(path)) {
    }

    void init();

    // IResourceProvider

    std::optional<ByteBuffer> findResourceData(const ResourceId &id) override;

    const ResourceIdSet &resourceIds() const override { return _resourceIds; }

    // END IResourceProvider

private:
    struct Resource {
        ResourceId id;
        uint32_t offset {0};
        uint32_t fileSize {0};
    };

    std::filesystem::path _path;
    std::unique_ptr<FileInputStream> _erf;

    ResourceIdSet _resourceIds;
    std::unordered_map<ResourceId, Resource, ResourceIdHasher> _idToResource;
};

} // namespace resource

} // namespace reone
