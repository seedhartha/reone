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

#include "reone/game/portraits.h"

#include "reone/resource/2da.h"
#include "reone/resource/2das.h"
#include "reone/resource/textures.h"


#include "reone/game/portrait.h"

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Portraits::init() {
    std::shared_ptr<TwoDa> portraits(_twoDas.get("portraits"));
    if (!portraits) {
        return;
    }

    for (int row = 0; row < portraits->getRowCount(); ++row) {
        std::string resRef(boost::to_lower_copy(portraits->getString(row, "baseresref")));

        Portrait portrait;
        portrait.resRef = resRef;
        portrait.appearanceNumber = portraits->getInt(row, "appearancenumber");
        portrait.appearanceS = portraits->getInt(row, "appearance_s");
        portrait.appearanceL = portraits->getInt(row, "appearance_l");
        portrait.forPC = portraits->getBool(row, "forpc");
        portrait.sex = portraits->getInt(row, "sex");

        _portraits.push_back(std::move(portrait));
    }
}

std::shared_ptr<Texture> Portraits::getTextureByIndex(int index) const {
    std::shared_ptr<Texture> result;
    if (index >= 0 && index < static_cast<int>(_portraits.size())) {
        result = getPortraitTexture(_portraits[index]);
    }
    return result;
}

std::shared_ptr<Texture> Portraits::getTextureByAppearance(int appearance) const {
    for (auto &portrait : _portraits) {
        if (portrait.appearanceNumber == appearance ||
            portrait.appearanceS == appearance ||
            portrait.appearanceL == appearance)
            return getPortraitTexture(portrait);
    }
    return nullptr;
}

std::shared_ptr<Texture> Portraits::getPortraitTexture(const Portrait &portrait) const {
    return _textures.get(portrait.resRef, TextureUsage::GUI);
}

} // namespace game

} // namespace reone
