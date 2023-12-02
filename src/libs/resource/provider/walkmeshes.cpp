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

#include "reone/resource/provider/walkmeshes.h"

#include "reone/graphics/format/bwmreader.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone::graphics;

namespace reone {

namespace resource {

Walkmeshes::Walkmeshes(Resources &resources) :
    _resources(resources) {
}

void Walkmeshes::clear() {
    _cache.clear();
}

std::shared_ptr<Walkmesh> Walkmeshes::get(const std::string &resRef, ResType type) {
    auto lcResRef = boost::to_lower_copy(resRef);

    auto maybeWalkmesh = _cache.find(lcResRef);
    if (maybeWalkmesh != _cache.end()) {
        return maybeWalkmesh->second;
    }
    auto inserted = _cache.insert(std::make_pair(lcResRef, doGet(lcResRef, type)));

    return inserted.first->second;
}

std::shared_ptr<Walkmesh> Walkmeshes::doGet(const std::string &resRef, ResType type) {
    auto res = _resources.find(ResourceId(resRef, type));
    if (!res) {
        return nullptr;
    }
    auto bwm = MemoryInputStream(res->data);
    auto reader = BwmReader(bwm);
    reader.load();
    return reader.walkmesh();
}

} // namespace resource

} // namespace reone
