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

#include "reone/game/d20/feats.h"

#include "reone/resource/2da.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/strings.h"

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Feats::init() {
    std::shared_ptr<TwoDA> feats(_twoDas.get("feat"));
    if (!feats) {
        return;
    }

    for (int row = 0; row < feats->getRowCount(); ++row) {
        std::string name(_strings.getText(feats->getInt(row, "name", -1)));
        std::string description(_strings.getText(feats->getInt(row, "description", -1)));
        std::shared_ptr<Texture> icon(_textures.get(feats->getString(row, "icon"), TextureUsage::GUI));
        uint32_t minCharLevel = feats->getHexUint(row, "mincharlevel");
        auto preReqFeat1 = static_cast<FeatType>(feats->getHexUint(row, "prereqfeat1"));
        auto preReqFeat2 = static_cast<FeatType>(feats->getHexUint(row, "prereqfeat2"));
        auto successor = static_cast<FeatType>(feats->getHexUint(row, "successor"));
        uint32_t pips = feats->getHexUint(row, "pips");

        auto feat = std::make_shared<Feat>();
        feat->name = std::move(name);
        feat->description = std::move(description);
        feat->icon = std::move(icon);
        feat->minCharLevel = minCharLevel;
        feat->preReqFeat1 = preReqFeat1;
        feat->preReqFeat2 = preReqFeat2;
        feat->successor = successor;
        feat->pips = pips;
        _feats.insert(std::make_pair(static_cast<FeatType>(row), std::move(feat)));
    }
}

std::shared_ptr<Feat> Feats::get(FeatType type) const {
    auto it = _feats.find(type);
    return it != _feats.end() ? it->second : nullptr;
}

} // namespace game

} // namespace reone
