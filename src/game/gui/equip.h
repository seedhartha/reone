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

#include "../../gui/gui.h"
#include "../../resource/types.h"

#include "../object/spatial.h"

namespace reone {

namespace game {

class EquipmentGui : public gui::GUI {
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

    EquipmentGui(resource::GameVersion version, const render::GraphicsOptions &opts);

    void load() override;
    void open(SpatialObject *owner);

    void setOnClose(const std::function<void()> &fn);

private:
    SpatialObject *_owner { nullptr };
    Slot _selectedSlot { Slot::None };
    int _selectedItemIdx { -1 };
    std::function<void()> _onClose;

    static resource::InventorySlot getInventorySlot(Slot slot);
    static std::shared_ptr<render::Texture> getEmptySlotIcon(Slot slot);

    void configureItemsListBox();
    void onItemClicked(const std::string &control, const std::string &item);
    void selectSlot(Slot slot);
    void updateEquipment();
    void updateItems();

    void preloadControl(gui::Control &control) override;
    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
