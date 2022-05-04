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

#include "../common/types.h"

#include "resourceprovider.h"
#include "types.h"

namespace reone {

namespace resource {

class Folder : public IResourceProvider {
public:
    Folder(int id = kDefaultProviderId) :
        _id(id) {
    }

    void load(const boost::filesystem::path &path);

    std::shared_ptr<ByteArray> find(const ResourceId &id) override;

    int id() const override { return _id; }

private:
    struct Resource {
        boost::filesystem::path path;
        ResourceType type;
    };

    int _id;

    boost::filesystem::path _path;
    std::multimap<std::string, Resource> _resources;

    void loadDirectory(const boost::filesystem::path &path);
};

} // namespace resource

} // namespace reone
