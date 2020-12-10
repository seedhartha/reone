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

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

Item::Item(uint32_t id) : Object(id, ObjectType::Item) {
}

void Item::load(const shared_ptr<ItemBlueprint> &blueprint) {
    blueprint->load(*this);
}

bool Item::isEquippable() const {
    return _equipableSlots != 0;
}

bool Item::isEquippable(InventorySlot slot) const {
    return (_equipableSlots >> slot) & 1;
}

const string &Item::localizedName() const {
    return _localizedName;
}

const string &Item::baseBodyVariation() const {
    return _baseBodyVariation;
}

int Item::bodyVariation() const {
    return _bodyVariation;
}

int Item::textureVariation() const {
    return _textureVariation;
}

const string &Item::itemClass() const {
    return _itemClass;
}

int Item::modelVariation() const {
    return _modelVariation;
}

shared_ptr<Texture> Item::icon() const {
    return _icon;
}

float Item::attackRange() const {
    return static_cast<float>(_attackRange);
}

WeaponType Item::weaponType() const {
    return _weaponType;
}

WeaponWield Item::weaponWield() const {
    return _weaponWield;
}

} // namespace game

} // namespace reone
