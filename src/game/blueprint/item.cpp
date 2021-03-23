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

#include "item.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../../audio/files.h"
#include "../../render/model/models.h"
#include "../../render/textures.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

#include "../object/item.h"

using namespace std;

using namespace reone::audio;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

ItemBlueprint::ItemBlueprint(const string &resRef, const shared_ptr<GffStruct> &uti) :
    _resRef(resRef),
    _uti(uti) {

    if (!uti) {
        throw invalid_argument("uti must not be null");
    }
}

void ItemBlueprint::load(Item &item) {
    item._blueprintResRef = _resRef;
    item._tag = boost::to_lower_copy(_uti->getString("Tag"));
    item._localizedName = Strings::instance().get(_uti->getInt("LocalizedName"));
    item._descIdentified = Strings::instance().get(_uti->getInt("DescIdentified"));

    shared_ptr<TwoDA> baseItems(Resources::instance().get2DA("baseitems"));
    item._baseItemType = _uti->getInt("BaseItem");
    item._equipableSlots = baseItems->getUint(item._baseItemType, "equipableslots", 0);

    string itemClass(baseItems->getString(item._baseItemType, "itemclass"));
    item._itemClass = boost::to_lower_copy(itemClass);

    string iconResRef;

    if (item.isEquippable(InventorySlot::body)) {
        item._baseBodyVariation = boost::to_lower_copy(baseItems->getString(item._baseItemType, "bodyvar"));
        item._bodyVariation = _uti->getInt("BodyVariation", 1);
        item._textureVariation = _uti->getInt("TextureVar", 1);

        iconResRef = str(boost::format("i%s_%03d") % item._itemClass % item._textureVariation);

    } else if (item.isEquippable(InventorySlot::rightWeapon)) {
        item._modelVariation = _uti->getInt("ModelVariation", 1);
        iconResRef = str(boost::format("i%s_%03d") % item._itemClass % item._modelVariation);

    } else {
        item._modelVariation = _uti->getInt("ModelVariation", 1);
        iconResRef = str(boost::format("i%s_%03d") % item._itemClass % item._modelVariation);
    }
    item._icon = Textures::instance().get(iconResRef, TextureUsage::GUI);

    item._attackRange = baseItems->getInt(item._baseItemType, "maxattackrange");
    item._numDice = baseItems->getInt(item._baseItemType, "numdice");
    item._dieToRoll = baseItems->getInt(item._baseItemType, "dietoroll");
    item._damageFlags = baseItems->getInt(item._baseItemType, "damageflags");
    item._weaponType = static_cast<WeaponType>(baseItems->getInt(item._baseItemType, "weapontype"));
    item._weaponWield = static_cast<WeaponWield>(baseItems->getInt(item._baseItemType, "weaponwield"));

    int ammunitionType = baseItems->getInt(item._baseItemType, "ammunitiontype", -1);
    if (ammunitionType != -1) {
        loadAmmunitionType(ammunitionType, item);
    }
}

void ItemBlueprint::loadAmmunitionType(int ordinal, Item &item) {
    shared_ptr<TwoDA> twoDa(Resources::instance().get2DA("ammunitiontypes"));
    item._ammunitionType = make_shared<Item::AmmunitionType>();
    item._ammunitionType->model = Models::instance().get(boost::to_lower_copy(twoDa->getString(ordinal, "model")));
    item._ammunitionType->shotSound1 = AudioFiles::instance().get(boost::to_lower_copy(twoDa->getString(ordinal, "shotsound0")));
    item._ammunitionType->shotSound2 = AudioFiles::instance().get(boost::to_lower_copy(twoDa->getString(ordinal, "shotsound1")));
    item._ammunitionType->impactSound1 = AudioFiles::instance().get(boost::to_lower_copy(twoDa->getString(ordinal, "impactsound0")));
    item._ammunitionType->impactSound2 = AudioFiles::instance().get(boost::to_lower_copy(twoDa->getString(ordinal, "impactsound1")));
}

} // namespace game

} // namespace reone
