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

#include "reone/resource/gffs.h"

#include "reone/system/stream/memoryinput.h"

#include "reone/resource/format/gffreader.h"
#include "reone/resource/resources.h"

namespace reone {

namespace resource {

std::shared_ptr<Gff> Gffs::get(const std::string &resRef, ResourceType type) {
    ResourceId id(resRef, type);
    auto maybeGff = _cache.find(id);
    if (maybeGff != _cache.end()) {
        return maybeGff->second;
    }
    std::shared_ptr<Gff> gff;
    auto maybeRaw = _resources.get(resRef, type);
    if (maybeRaw) {
        auto stream = MemoryInputStream(*maybeRaw);
        auto reader = GffReader();
        reader.load(stream);
        gff = reader.root();
    }
    auto inserted = _cache.insert(std::make_pair(id, gff));
    return inserted.first->second;
}

} // namespace resource

} // namespace reone
