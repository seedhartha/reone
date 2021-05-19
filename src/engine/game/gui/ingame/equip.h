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

#include "../gui.h"

namespace reone {

namespace game {

class Creature;

class Equipment : public GameGUI {
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

    Equipment(Game *game);

    void load() override;

    void update();

private:
    Slot _selectedSlot { Slot::None };
    int _selectedItemIdx { -1 };

    void onClick(const std::string &control) override;
    void onFocusChanged(const std::string &control, bool focus) override;
    void onListBoxItemClick(const std::string &control, const std::string &item) override;
    void preloadControl(gui::Control &control) override;

    void configureItemsListBox();
    void updateEquipment();
    void updateItems();
    void updatePortraits();
    void selectSlot(Slot slot);

    std::shared_ptr<graphics::Texture> getItemFrameTexture(int stackSize) const;
};

} // namespace game

} // namespace reone
