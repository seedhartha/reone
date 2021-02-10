
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

#include "lips.h"

#include "../../common/streamutil.h"
#include "../../resource/resources.h"

#include "lipfile.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

Lips &Lips::instance() {
    static Lips instance;
    return instance;
}

void Lips::invalidateCache() {
    _cache.clear();
}

shared_ptr<LipAnimation> Lips::get(const string &resRef) {
    auto maybeAnimation = _cache.find(resRef);
    if (maybeAnimation != _cache.end()) return maybeAnimation->second;

    shared_ptr<LipAnimation> animation;
    shared_ptr<ByteArray> lipData(Resources::instance().get(resRef, ResourceType::Lip));
    if (lipData) {
        LipFile lip;
        lip.load(wrap(lipData));
        animation = lip.animation();
    }

    return _cache.insert(make_pair(resRef, move(animation))).first->second;
}

} // namespace render

} // namespace reone
