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

#include "reone/game/object/item.h"

#include "reone/audio/di/services.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/graphics/di/services.h"
#include "reone/resource/2da.h"
#include "reone/resource/audioplayer.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/audiofiles.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"


using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Item::loadFromBlueprint(const std::string &resRef) {
    std::shared_ptr<Gff> uti(_services.resource.gffs.get(resRef, ResType::Uti));
    if (uti) {
        auto utiParsed = resource::generated::parseUTI(*uti);
        loadUTI(utiParsed);
    }
}

void Item::update(float dt) {
    if (_audioSource) {
        _audioSource->update();
    }
}

void Item::playShotSound(int variant, glm::vec3 position) {
    if (!_ammunitionType) {
        return;
    }
    std::shared_ptr<AudioBuffer> sound(variant == 1 ? _ammunitionType->shotSound2 : _ammunitionType->shotSound1);
    if (sound) {
        _audioSource = _services.resource.player.play(sound, AudioType::Sound, false, 1.0f, true, std::move(position));
    }
}

void Item::playImpactSound(int variant, glm::vec3 position) {
    if (!_ammunitionType) {
        return;
    }
    std::shared_ptr<AudioBuffer> sound(variant == 1 ? _ammunitionType->impactSound2 : _ammunitionType->impactSound1);
    if (sound) {
        _services.resource.player.play(sound, AudioType::Sound, false, 1.0f, true, std::move(position));
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

void Item::loadUTI(const resource::generated::UTI &uti) {
    _blueprintResRef = boost::to_lower_copy(uti.TemplateResRef);
    _baseItem = uti.BaseItem; // index into baseitems.2da
    _localizedName = _services.resource.strings.getText(uti.LocalizedName.first);
    _description = _services.resource.strings.getText(uti.Description.first);
    _descIdentified = _services.resource.strings.getText(uti.DescIdentified.first);
    _tag = boost::to_lower_copy(uti.Tag);
    _charges = uti.Charges;
    _cost = uti.Cost;
    _stolen = uti.Stolen;
    _stackSize = uti.StackSize;
    _plot = uti.Plot;
    _addCost = uti.AddCost;
    _identified = uti.Identified;
    _modelVariation = uti.ModelVariation;
    _textureVariation = uti.TextureVar;
    _bodyVariation = uti.BodyVariation;

    std::shared_ptr<TwoDa> baseItems(_services.resource.twoDas.get("baseitems"));
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

    std::string iconResRef;
    if (isEquippable(InventorySlot::body)) {
        _baseBodyVariation = boost::to_lower_copy(baseItems->getString(_baseItem, "bodyvar"));
        iconResRef = str(boost::format("i%s_%03d") % _itemClass % _textureVariation);
    } else if (isEquippable(InventorySlot::rightWeapon)) {
        iconResRef = str(boost::format("i%s_%03d") % _itemClass % _modelVariation);
    } else {
        iconResRef = str(boost::format("i%s_%03d") % _itemClass % _modelVariation);
    }
    _icon = _services.resource.textures.get(iconResRef, TextureUsage::GUI);

    loadAmmunitionType();

    // TODO: load properties

    // Unused fields:
    //
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Item::loadAmmunitionType() {
    std::shared_ptr<TwoDa> baseItems(_services.resource.twoDas.get("baseitems"));

    int ammunitionIdx = baseItems->getInt(_baseItem, "ammunitiontype", -1);
    if (ammunitionIdx != -1) {
        std::shared_ptr<TwoDa> twoDa(_services.resource.twoDas.get("ammunitiontypes"));
        _ammunitionType = std::make_shared<Item::AmmunitionType>();
        _ammunitionType->model = _services.resource.models.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "model")));
        _ammunitionType->shotSound1 = _services.resource.files.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "shotsound0")));
        _ammunitionType->shotSound2 = _services.resource.files.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "shotsound1")));
        _ammunitionType->impactSound1 = _services.resource.files.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "impactsound0")));
        _ammunitionType->impactSound2 = _services.resource.files.get(boost::to_lower_copy(twoDa->getString(ammunitionIdx, "impactsound1")));
    }
}

} // namespace game

} // namespace reone
