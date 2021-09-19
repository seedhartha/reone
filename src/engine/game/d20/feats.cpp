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

#include "feats.h"

#include "../../common/collectionutil.h"
#include "../../di/services/resource.h"
#include "../../resource/2da.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

Feats::Feats(Textures &textures, ResourceServices &resource) :
    _textures(textures),
    _resource(resource) {
}

void Feats::init() {
    shared_ptr<TwoDA> feats(_resource.resources().get2DA("feat"));
    if (!feats) return;

    for (int row = 0; row < feats->getRowCount(); ++row) {
        string name(_resource.strings().get(feats->getInt(row, "name", -1)));
        string description(_resource.strings().get(feats->getInt(row, "description", -1)));
        shared_ptr<Texture> icon(_textures.get(feats->getString(row, "icon"), TextureUsage::GUI));
        uint32_t minCharLevel = feats->getUint(row, "mincharlevel");
        auto preReqFeat1 = static_cast<FeatType>(feats->getUint(row, "prereqfeat1"));
        auto preReqFeat2 = static_cast<FeatType>(feats->getUint(row, "prereqfeat2"));
        auto successor = static_cast<FeatType>(feats->getUint(row, "successor"));
        uint32_t pips = feats->getUint(row, "pips");

        auto feat = make_shared<Feat>();
        feat->name = move(name);
        feat->description = move(description);
        feat->icon = move(icon);
        feat->minCharLevel = minCharLevel;
        feat->preReqFeat1 = preReqFeat1;
        feat->preReqFeat2 = preReqFeat2;
        feat->successor = successor;
        feat->pips = pips;
        _feats.insert(make_pair(static_cast<FeatType>(row), move(feat)));
    }
}

shared_ptr<Feat> Feats::get(FeatType type) const {
    return getFromLookupOrNull(_feats, type);
}

} // namespace game

} // namespace reone
