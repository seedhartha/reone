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

#include "../../render/model/model.h"
#include "../../render/texture.h"
#include "../../resource/gfffile.h"

#include "../blueprint/item.h"
#include "../types.h"

#include "object.h"

namespace reone {

namespace game {

class Item : public Object {
public:
    struct AmmunitionType {
        std::shared_ptr<render::Model> model;
    };

    Item(uint32_t id);

    void load(const std::shared_ptr<ItemBlueprint> &blueprint);

    bool isEquippable() const;
    bool isEquippable(InventorySlot slot) const;
    bool isDropable() const;
    bool isIdentified() const;
    bool isEquipped() const;

    const std::string &localizedName() const;
    const std::string &baseBodyVariation() const;
    int bodyVariation() const;
    int textureVariation() const;
    const std::string &itemClass() const;
    int modelVariation() const;
    std::shared_ptr<render::Texture> icon() const;
    float attackRange() const;
    int numDice() const;
    int dieToRoll() const;
    int damageFlags() const;
    WeaponType weaponType() const;
    WeaponWield weaponWield() const;
    int stackSize() const;
    std::shared_ptr<AmmunitionType> ammunitionType() const;

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

    friend class ItemBlueprint;
};

} // namespace game

} // namespace reone
