/*
 * Copyright (c) 2020 The reone project contributors
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

#include <set>

#include <boost/filesystem.hpp>

#include "../common/types.h"

#include "types.h"

namespace reone {

namespace resource {

class Folder : public IResourceProvider {
public:
    Folder() = default;
    void load(const boost::filesystem::path &path);

    bool supports(ResourceType type) const override;
    std::shared_ptr<ByteArray> find(const std::string &resRef, ResourceType type) override;

private:
    struct Resource {
        boost::filesystem::path path;
        ResourceType type;
    };

    boost::filesystem::path _path;
    std::multimap<std::string, Resource> _resources;

    Folder(const Folder &) = delete;
    Folder &operator=(const Folder &) = delete;

    void loadDirectory(const boost::filesystem::path &path);
};

} // namespace resource

} // namespace reone
