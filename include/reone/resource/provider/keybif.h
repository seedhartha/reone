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

class BifReader;

class KeyBifResourceProvider : public IResourceProvider {
public:
    struct Resource {
        int bifIdx {0};
        uint32_t bifOffset {0};
        uint32_t fileSize {0};
    };

    KeyBifResourceProvider(boost::filesystem::path keyPath, int id = kDefaultProviderId) :
        _keyPath(std::move(keyPath)),
        _id(id) {
    }

    void init();

    const std::unordered_map<ResourceId, Resource, ResourceIdHasher> &resources() const { return _resources; }

    // IResourceProvider

    std::shared_ptr<ByteBuffer> find(const ResourceId &id) override;

    int id() const override { return _id; }

    // END IResourceProvider

private:
    boost::filesystem::path _keyPath;
    int _id;

    std::vector<boost::filesystem::path> _bifPaths;
    std::unordered_map<ResourceId, Resource, ResourceIdHasher> _resources;
};

} // namespace resource

} // namespace reone
