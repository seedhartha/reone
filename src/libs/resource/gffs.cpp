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

#include "reone/resource/format/gffreader.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryinput.h"

namespace reone {

namespace resource {

std::shared_ptr<Gff> Gffs::get(const std::string &resRef, ResType type) {
    ResourceId resId(resRef, type);
    return _cache.getOrAdd(resId, [this, &resId]() {
        auto res = _resources.find(resId);
        if (!res) {
            return std::shared_ptr<Gff>();
        }
        MemoryInputStream stream(res->data);
        GffReader reader(stream);
        reader.load();
        return reader.root();
    });
}

} // namespace resource

} // namespace reone
