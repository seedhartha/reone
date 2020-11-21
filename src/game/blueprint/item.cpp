/*
 * Copyright (c) 2020 The reone project contributors
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

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../../render/textures.h"
#include "../../resource/resources.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

ItemBlueprint::ItemBlueprint(const string &resRef) : _resRef(resRef) {
}

void ItemBlueprint::load(const GffStruct &uti) {
    _tag = uti.getString("Tag");
    boost::to_lower(_tag);

    _localizedName = Resources::instance().getString(uti.getInt("LocalizedName"));

    shared_ptr<TwoDaTable> baseItems(Resources::instance().get2DA("baseitems"));
    int baseItem = uti.getInt("BaseItem");
    _equipableSlots = baseItems->getUint(baseItem, "equipableslots", 0);

    string itemClass(baseItems->getString(baseItem, "itemclass"));
    _itemClass = boost::to_lower_copy(itemClass);

    string iconResRef;

    if (isEquippable(kInventorySlotBody)) {
        _baseBodyVariation = baseItems->getString(baseItem, "bodyvar");
        boost::to_lower(_baseBodyVariation);

        _bodyVariation = uti.getInt("BodyVariation", 1);
        _textureVariation = uti.getInt("TextureVar", 1);

        iconResRef = str(boost::format("i%s_%03d") % _itemClass % _textureVariation);

    } else if (isEquippable(kInventorySlotRightWeapon)) {
        _modelVariation = uti.getInt("ModelVariation", 1);
        iconResRef = str(boost::format("i%s_%03d") % _itemClass % _modelVariation);

    } else {
        _modelVariation = uti.getInt("ModelVariation", 1);
        iconResRef = str(boost::format("i%s_%03d") % _itemClass % _modelVariation);
    }
    _icon = Textures::instance().get(iconResRef, TextureType::GUI);

    _attackRange = baseItems->getInt(baseItem, "maxattackrange");
    _weaponType = static_cast<WeaponType>(baseItems->getInt(baseItem, "weapontype"));
    _weaponWield = static_cast<WeaponWield>(baseItems->getInt(baseItem, "weaponwield"));
}

bool ItemBlueprint::isEquippable() const {
    return _equipableSlots != 0;
}

bool ItemBlueprint::isEquippable(InventorySlot slot) const {
    return (_equipableSlots >> slot) & 1;
}

const string &ItemBlueprint::resRef() const {
    return _resRef;
}

const string &ItemBlueprint::tag() const {
    return _tag;
}

const string &ItemBlueprint::localizedName() const {
    return _localizedName;
}

const string &ItemBlueprint::baseBodyVariation() const {
    return _baseBodyVariation;
}

int ItemBlueprint::bodyVariation() const {
    return _bodyVariation;
}

int ItemBlueprint::textureVariation() const {
    return _textureVariation;
}

const string &ItemBlueprint::itemClass() const {
    return _itemClass;
}

int ItemBlueprint::modelVariation() const {
    return _modelVariation;
}

shared_ptr<Texture> ItemBlueprint::icon() const {
    return _icon;
}

int ItemBlueprint::attackRange() const {
    return _attackRange;
}

WeaponType ItemBlueprint::weaponType() const {
    return _weaponType;
}

WeaponWield ItemBlueprint::weaponWield() const {
    return _weaponWield;
}

} // namespace game

} // namespace reone
