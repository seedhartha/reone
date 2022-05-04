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

#include "walkmeshes.h"

#include "../common/stream/bytearrayinput.h"
#include "../resource/resources.h"

#include "format/bwmreader.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

Walkmeshes::Walkmeshes(Resources &resources) :
    _resources(resources) {
}

void Walkmeshes::invalidate() {
    _cache.clear();
}

shared_ptr<Walkmesh> Walkmeshes::get(const string &resRef, ResourceType type) {
    auto lcResRef = boost::to_lower_copy(resRef);

    auto maybeWalkmesh = _cache.find(lcResRef);
    if (maybeWalkmesh != _cache.end()) {
        return maybeWalkmesh->second;
    }
    auto inserted = _cache.insert(make_pair(lcResRef, doGet(lcResRef, type)));

    return inserted.first->second;
}

shared_ptr<Walkmesh> Walkmeshes::doGet(const string &resRef, ResourceType type) {
    shared_ptr<ByteArray> data(_resources.get(resRef, type));
    shared_ptr<Walkmesh> walkmesh;

    if (data) {
        BwmReader bwm;
        bwm.load(ByteArrayInputStream(*data));
        walkmesh = bwm.walkmesh();
    }

    return move(walkmesh);
}

} // namespace graphics

} // namespace reone
