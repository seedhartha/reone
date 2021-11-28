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

#include "../../audio/files.h"
#include "../../audio/player.h"
#include "../../graphics/models.h"
#include "../../graphics/textures.h"
#include "../../resource/2da.h"
#include "../../resource/2das.h"
#include "../../resource/gffs.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

#include "../game.h"
#include "../services.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Item::loadFromBlueprint(const string &resRef) {
    shared_ptr<GffStruct> uti(_services.gffs.get(resRef, ResourceType::Uti));
    if (uti) {
        loadUTI(*uti);
    }
}

void Item::playShotSound(int variant, glm::vec3 position) {
    if (_ammunitionType) {
        shared_ptr<AudioStream> sound(variant == 1 ? _ammunitionType->shotSound2 : _ammunitionType->shotSound1);
        if (sound) {
            _services.audioPlayer.play(sound, AudioType::Sound, false, 1.0f, true, move(position));
        }
    }
}

void Item::playImpactSound(int variant, glm::vec3 position) {
    if (_ammunitionType) {
        shared_ptr<AudioStream> sound(variant == 1 ? _ammunitionType->impactSound2 : _ammunitionType->impactSound1);
        _services.audioPlayer.play(sound, AudioType::Sound, false, 1.0f, true, move(position));
    }
}

bool Item::isEquippable() const {
    return _equipableSlots != 0;
}

bool Item::isEquippable(int slot) const {
    return (_equipableSlots >> slot) & 1;
}

void Item::setDropable(bool dropable) {
    _dropable = dropable;
}

void Item::setStackSize(int stackSize) {
    _stackSize = stackSize;
}

void Item::setIdentified(bool value) {
    _identified = value;
}

void Item::setEquipped(bool equipped) {
    _equipped = equipped;
}

void Item::loadUTI(const GffStruct &uti) {
    _blueprintResRef = boost::to_lower_copy(uti.getString("TemplateResRef"));
    _baseItem = uti.getInt("BaseItem"); // index into baseitems.2da
    _localizedName = _services.strings.get(uti.getInt("LocalizedName"));
    _description = _services.strings.get(uti.getInt("Description"));
    _descIdentified = _services.strings.get(uti.getInt("DescIdentified"));
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

    shared_ptr<TwoDA> baseItems(_services.twoDas.get("baseitems"));
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
    _icon = _services.textures.get(iconResRef, TextureUsage::GUI);

    loadAmmunitionType();

    // TODO: load properties

    // Unused fields:
    //
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Item::loadAmmunitionType() {
    shared_ptr<TwoDA> baseItems(_services.twoDas.get("baseitems"));

    int ammunitionIdx = baseItems->getInt(_baseItem, "ammunitiontype", -1);
    if (ammunitionIdx != -1) {
        shared_ptr<TwoDA> twoDa(_services.twoDas.get("ammunitiontypes"));
        _ammunitionType = make_shared<Item::AmmunitionType>();
        _ammunitionType->model = _services.models.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "model")));
        _ammunitionType->shotSound1 = _services.audioFiles.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "shotsound0")));
        _ammunitionType->shotSound2 = _services.audioFiles.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "shotsound1")));
        _ammunitionType->impactSound1 = _services.audioFiles.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "impactsound0")));
        _ammunitionType->impactSound2 = _services.audioFiles.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "impactsound1")));
    }
}

} // namespace game

} // namespace reone
