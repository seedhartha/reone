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

class MemoryResourceProvider : public IResourceProvider, boost::noncopyable {
public:
    void clear() {
        _resourceIds.clear();
        _idToResource.clear();
    }

    void add(const ResourceId &id, ByteBuffer data) {
        _resourceIds.insert(id);
        _idToResource.insert(std::make_pair(id, std::move(data)));
    }

    // IResourceProvider

    std::optional<ByteBuffer> findResourceData(const ResourceId &id) override {
        auto it = _idToResource.find(id);
        if (it == _idToResource.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    const ResourceIdSet &resourceIds() const override { return _resourceIds; }

    // END IResourceProvider

private:
    ResourceIdSet _resourceIds;
    std::unordered_map<ResourceId, ByteBuffer, ResourceIdHasher> _idToResource;
};

} // namespace resource

} // namespace reone
