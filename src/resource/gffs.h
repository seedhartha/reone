/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "gff.h"
#include "id.h"
#include "types.h"

namespace reone {

namespace resource {

class Resources;

class Gffs {
public:
    Gffs(Resources &resources) :
        _resources(resources) {
    }

    void invalidate() {
        _cache.clear();
    }

    std::shared_ptr<Gff> get(const std::string &resRef, ResourceType type);

    void add(ResourceId resId, std::shared_ptr<Gff> gff) {
        _cache[resId] = std::move(gff);
    }

private:
    Resources &_resources;

    std::unordered_map<ResourceId, std::shared_ptr<Gff>, ResourceIdHasher> _cache;
};

} // namespace resource

} // namespace reone
