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

namespace generated {

struct UTC;

struct AppearanceTwoDA;
struct HeadsTwoDA;

} // namespace generated

} // namespace resource

namespace game {

namespace neo {

struct Appearance {
    resource::ResRef model;
    std::optional<resource::ResRef> texture;
    std::optional<resource::ResRef> normalHeadModel;
    std::optional<resource::ResRef> backupHeadModel;
    // TODO: evil head textures
};

class Item;

using EquipmentMap = std::map<InventorySlot, std::reference_wrapper<Item>>;

class Creature : public SpatialObject {
public:
    enum class MoveType {
        Invalid = -1,
        None,
        Walk,
        Run
    };

    Creature(ObjectId id, ObjectTag tag) :
        SpatialObject(
            id,
            std::move(tag),
            ObjectType::Creature) {
    }

    void load(const resource::generated::UTC &utc,
              const resource::generated::AppearanceTwoDA &appearance,
              const resource::generated::HeadsTwoDA &heads);

    void load(AppearanceId appearanceId,
              const resource::generated::AppearanceTwoDA &appearance,
              const resource::generated::HeadsTwoDA &heads,
              std::optional<int> bodyVariation = std::nullopt,
              std::optional<int> texVariation = std::nullopt);

    const Appearance &appearance() const {
        return _appearance;
    }

    // Movement

    MoveType moveType() const {
        return _moveType;
    }

    void setMoveType(MoveType type);

    // END Movement

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
    MoveType _moveType {MoveType::Invalid};
    Inventory _inventory;
    EquipmentMap _equipment;
};

} // namespace neo

} // namespace game

} // namespace reone
