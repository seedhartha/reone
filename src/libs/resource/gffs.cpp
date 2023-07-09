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
    auto resId = ResourceId(resRef, type);
    auto cached = _cache.find(resId);
    if (cached != _cache.end()) {
        return cached->second;
    }
    std::shared_ptr<Gff> gff;
    auto data = _resources.find(resId);
    if (data) {
        auto stream = MemoryInputStream(*data);
        auto reader = GffReader(stream);
        reader.load();
        gff = reader.root();
    }
    auto inserted = _cache.insert(std::make_pair(resId, gff));
    return inserted.first->second;
}

} // namespace resource

} // namespace reone
