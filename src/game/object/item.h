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

#pragma once

#include <memory>

#include "../../audio/stream.h"
#include "../../render/model/model.h"
#include "../../render/texture.h"
#include "../../resource/format/gffreader.h"

#include "../types.h"

#include "object.h"

namespace reone {

namespace game {

class Item : public Object {
public:
    struct AmmunitionType {
        std::shared_ptr<render::Model> model;
        std::shared_ptr<audio::AudioStream> shotSound1;
        std::shared_ptr<audio::AudioStream> shotSound2;
        std::shared_ptr<audio::AudioStream> impactSound1;
        std::shared_ptr<audio::AudioStream> impactSound2;
    };

    Item(uint32_t id);

    void loadFromBlueprint(const std::string &resRef);

    void playShotSound(int variant, glm::vec3 position);
    void playImpactSound(int variant, glm::vec3 position);

    bool isEquippable() const;
    bool isEquippable(int slot) const;
    bool isDropable() const { return _dropable; }
    bool isIdentified() const { return _identified; }
    bool isEquipped() const { return _equipped; }

    const std::string &baseBodyVariation() const { return _baseBodyVariation; }
    const std::string &itemClass() const { return _itemClass; }
    const std::string &localizedName() const { return _localizedName; }
    float attackRange() const { return static_cast<float>(_attackRange); }
    int bodyVariation() const { return _bodyVariation; }
    int damageFlags() const { return _damageFlags; }
    int dieToRoll() const { return _dieToRoll; }
    int modelVariation() const { return _modelVariation; }
    int numDice() const { return _numDice; }
    int stackSize() const { return _stackSize; }
    int textureVariation() const { return _textureVariation; }
    std::shared_ptr<AmmunitionType> ammunitionType() const { return _ammunitionType; }
    std::shared_ptr<render::Texture> icon() const { return _icon; }
    WeaponType weaponType() const { return _weaponType; }
    WeaponWield weaponWield() const { return _weaponWield; }
    const std::string &descIdentified() const { return _descIdentified; }
    int baseItemType() const { return _baseItemType; }

    void setDropable(bool dropable);
    void setStackSize(int size);
    void setIdentified(bool value);
    void setEquipped(bool equipped);

private:
    std::string _localizedName;
    std::string _baseBodyVariation;
    int _bodyVariation { 0 };
    int _textureVariation { 0 };
    std::string _itemClass;
    int _modelVariation { 0 };
    std::shared_ptr<render::Texture> _icon;
    uint32_t _equipableSlots { 0 };
    int _attackRange { 0 };
    int _numDice { 0 };
    int _dieToRoll { 0 };
    int _damageFlags { 0 };
    WeaponType _weaponType { WeaponType::None };
    WeaponWield _weaponWield { WeaponWield::None };
    bool _dropable { true };
    int _stackSize { 1 };
    bool _identified { true };
    bool _equipped { false };
    std::shared_ptr<AmmunitionType> _ammunitionType;
    std::string _descIdentified;
    int _baseItemType { 0 };

    // Blueprint

    void loadUTI(const resource::GffStruct &uti);

    void loadAmmunitionTypeFromUTI(const resource::GffStruct &uti);

    // END Blueprint
};

} // namespace game

} // namespace reone
