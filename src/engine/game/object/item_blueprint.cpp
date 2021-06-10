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

/** @file
 *  Item functions related to blueprint loading.
 */

#include "item.h"

#include "../../audio/files.h"
#include "../../graphics/model/models.h"
#include "../../graphics/texture/textures.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

#include "../game.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Item::loadUTI(const GffStruct &uti) {
    _blueprintResRef = boost::to_lower_copy(uti.getString("TemplateResRef"));
    _baseItem = uti.getInt("BaseItem"); // index into baseitems.2da
    _localizedName = _game->services().resource().strings().get(uti.getInt("LocalizedName"));
    _description = _game->services().resource().strings().get(uti.getInt("Description"));
    _descIdentified = _game->services().resource().strings().get(uti.getInt("DescIdentified"));
    _tag = boost::to_lower_copy(uti.getString("Tag"));
    _charges = uti.getInt("Charges");
    _cost = uti.getInt("Cost");
    _stolen = uti.getBool("Stolen");
    _stackSize = uti.getInt("StackSize");
    _plot = uti.getBool("Plot");
    _addCost = uti.getInt("AddCost");
    _identified = uti.getInt("Identified");
    _modelVariation = uti.getInt("ModelVariation", 1);
    _textureVariation = uti.getInt("TextureVar", 1);
    _bodyVariation = uti.getInt("BodyVariation", 1);

    shared_ptr<TwoDA> baseItems(_game->services().resource().resources().get2DA("baseitems"));
    _attackRange = baseItems->getInt(_baseItem, "maxattackrange");
    _criticalHitMultiplier = baseItems->getInt(_baseItem, "crithitmult");
    _criticalThreat = baseItems->getInt(_baseItem, "critthreat");
    _damageFlags = baseItems->getInt(_baseItem, "damageflags");
    _dieToRoll = baseItems->getInt(_baseItem, "dietoroll");
    _equipableSlots = baseItems->getUint(_baseItem, "equipableslots", 0);
    _itemClass = boost::to_lower_copy(baseItems->getString(_baseItem, "itemclass"));
    _numDice = baseItems->getInt(_baseItem, "numdice");
    _weaponType = static_cast<WeaponType>(baseItems->getInt(_baseItem, "weapontype"));
    _weaponWield = static_cast<WeaponWield>(baseItems->getInt(_baseItem, "weaponwield"));

    string iconResRef;
    if (isEquippable(InventorySlot::body)) {
        _baseBodyVariation = boost::to_lower_copy(baseItems->getString(_baseItem, "bodyvar"));
        iconResRef = str(boost::format("i%s_%03d") % _itemClass % _textureVariation);
    } else if (isEquippable(InventorySlot::rightWeapon)) {
        iconResRef = str(boost::format("i%s_%03d") % _itemClass % _modelVariation);
    } else {
        iconResRef = str(boost::format("i%s_%03d") % _itemClass % _modelVariation);
    }
    _icon = _game->services().graphics().textures().get(iconResRef, TextureUsage::GUI);

    loadAmmunitionType();

    // TODO: load properties

    // Unused fields:
    //
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Item::loadAmmunitionType() {
    shared_ptr<TwoDA> baseItems(_game->services().resource().resources().get2DA("baseitems"));

    int ammunitionIdx = baseItems->getInt(_baseItem, "ammunitiontype", -1);
    if (ammunitionIdx != -1) {
        shared_ptr<TwoDA> twoDa(_game->services().resource().resources().get2DA("ammunitiontypes"));
        _ammunitionType = make_shared<Item::AmmunitionType>();
        _ammunitionType->model = _game->services().graphics().models().get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "model")));
        _ammunitionType->shotSound1 = _game->services().audio().files().get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "shotsound0")));
        _ammunitionType->shotSound2 = _game->services().audio().files().get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "shotsound1")));
        _ammunitionType->impactSound1 = _game->services().audio().files().get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "impactsound0")));
        _ammunitionType->impactSound2 = _game->services().audio().files().get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "impactsound1")));
    }
}

} // namespace game

} // namespace reone
