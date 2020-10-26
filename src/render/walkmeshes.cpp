/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../resource/resources.h"
#include "../system/streamutil.h"

#include "bwmfile.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

Walkmeshes &Walkmeshes::instance() {
    static Walkmeshes instance;
    return instance;
}

void Walkmeshes::invalidateCache() {
    _cache.clear();
}

shared_ptr<Walkmesh> Walkmeshes::get(const string &resRef, ResourceType type) {
    auto maybeWalkmesh = _cache.find(resRef);
    if (maybeWalkmesh != _cache.end()) {
        return maybeWalkmesh->second;
    }
    auto inserted = _cache.insert(make_pair(resRef, doGet(resRef, type)));

    return inserted.first->second;
}

shared_ptr<Walkmesh> Walkmeshes::doGet(const string &resRef, ResourceType type) {
    shared_ptr<ByteArray> data(Resources::instance().findRaw(resRef, type));
    shared_ptr<Walkmesh> walkmesh;

    if (data) {
        BwmFile bwm;
        bwm.load(wrap(data));
        walkmesh = bwm.walkmesh();
    }

    return move(walkmesh);
}

} // namespace render

} // namespace reone
