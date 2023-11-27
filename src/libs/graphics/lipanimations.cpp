
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

#include "reone/graphics/lips.h"

#include "reone/graphics/format/lipreader.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone::resource;

namespace reone {

namespace graphics {

std::shared_ptr<LipAnimation> Lips::doGet(std::string resRef) {
    auto res = _resources.find(ResourceId(resRef, ResType::Lip));
    if (!res) {
        return nullptr;
    }
    auto stream = MemoryInputStream(res->data);
    auto reader = LipReader(stream, resRef);
    reader.load();
    return reader.animation();
}

} // namespace graphics

} // namespace reone
