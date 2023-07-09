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

#include "types.h"
#include "typeutil.h"

namespace reone {

namespace resource {

struct ResourceId {
    std::string resRef;
    ResourceType type {ResourceType::Invalid};

    ResourceId() = default;

    ResourceId(std::string resRef, ResourceType type) :
        resRef(std::move(resRef)),
        type(type) {
    }

    std::string string() const {
        return resRef + "." + getExtByResType(type);
    }

    bool operator==(const ResourceId &other) const {
        return resRef == other.resRef && type == other.type;
    }

    bool operator!=(const ResourceId &other) const {
        return !operator==(other);
    }
};

struct ResourceIdHasher {
    size_t operator()(const ResourceId &id) const {
        size_t seed = 0;
        boost::hash_combine(seed, id.resRef);
        boost::hash_combine(seed, id.type);
        return seed;
    }
};

typedef std::unordered_set<ResourceId, ResourceIdHasher> ResourceIdSet;

} // namespace resource

} // namespace reone
