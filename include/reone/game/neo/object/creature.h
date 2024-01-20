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

#pragma once

#include "reone/resource/resref.h"

#include "../inventory.h"
#include "../object.h"

namespace reone {

namespace resource {

class TwoDA;

namespace generated {

class UTC;

}

} // namespace resource

namespace game {

namespace neo {

struct Appearance {
    resource::ResRef model;
    resource::ResRef texture;
    std::optional<resource::ResRef> normalHeadModel;
    std::optional<resource::ResRef> backupHeadModel;
    // TODO: evil head textures
};

class Item;

using EquipmentMap = std::map<InventorySlot, std::reference_wrapper<Item>>;

class Creature : public SpatialObject {
public:
    Creature(ObjectId id, ObjectTag tag) :
        SpatialObject(
            id,
            std::move(tag),
            ObjectType::Creature) {
    }

    void load(const resource::generated::UTC &utc,
              const resource::TwoDA &appearance2da,
              const resource::TwoDA &heads2da);

    const Appearance &appearance() const {
        return _appearance;
    }

    // Items

    void give(Item &item) {
        _inventory.add(item);
    }

    const ItemList &items() const {
        return _inventory.items();
    }

    // END Items

    // Equipment

    void equip(Item &item, InventorySlot slot);
    void unequip(const Item &item);

    const EquipmentMap &equipment() const {
        return _equipment;
    }

    // END Equipment

private:
    Appearance _appearance;
    Inventory _inventory;
    EquipmentMap _equipment;
};

} // namespace neo

} // namespace game

} // namespace reone
