/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../../system/gui/gui.h"
#include "../../system/resource/types.h"

#include "../object/spatial.h"

namespace reone {

namespace game {

class Game;

class Equipment : public gui::GUI {
public:
    enum class Slot {
        None,
        Implant,
        Head,
        Hands,
        ArmL,
        Body,
        ArmR,
        WeapL,
        Belt,
        WeapR,
        WeapL2,
        WeapR2
    };

    Equipment(Game *game, resource::GameVersion version, const render::GraphicsOptions &opts);

    void load() override;
    void open(SpatialObject *owner);

private:
    Game *_game { nullptr };
    SpatialObject *_owner { nullptr };
    Slot _selectedSlot { Slot::None };
    int _selectedItemIdx { -1 };

    static resource::InventorySlot getInventorySlot(Slot slot);
    static std::shared_ptr<render::Texture> getEmptySlotIcon(Slot slot);

    void configureItemsListBox();
    void selectSlot(Slot slot);
    void updateEquipment();
    void updateItems();

    void onClick(const std::string &control) override;
    void onListBoxItemClick(const std::string &control, const std::string &item) override;
    void preloadControl(gui::Control &control) override;
};

} // namespace game

} // namespace reone
