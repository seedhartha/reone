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
#include "../../../gui/control/button.h"
#include "../../../gui/control/imagebutton.h"
#include "../../../gui/control/label.h"
#include "../../../gui/control/listbox.h"

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
    struct Binding {
        std::shared_ptr<gui::Label> lblCantEquip { nullptr };
        std::shared_ptr<gui::Label> lblAttackInfo { nullptr };
        std::shared_ptr<gui::Label> lblToHitR { nullptr };
        std::shared_ptr<gui::Label> lblPortBord { nullptr };
        std::shared_ptr<gui::Label> lblPortrait { nullptr };
        std::shared_ptr<gui::ListBox> lbItems { nullptr };
        std::shared_ptr<gui::Label> lblDefInfo { nullptr };
        std::unordered_map<Equipment::Slot, std::shared_ptr<gui::Label>> lblInv;
        std::unordered_map<Equipment::Slot, std::shared_ptr<gui::Button>> btnInv;
        std::shared_ptr<gui::Label> lblAtkL { nullptr };
        std::shared_ptr<gui::Label> lblAtkR { nullptr };
        std::shared_ptr<gui::Label> lblDef { nullptr };
        std::shared_ptr<gui::Label> lblTitle { nullptr };
        std::shared_ptr<gui::Label> lblDamage { nullptr };
        std::shared_ptr<gui::Label> lblToHitL { nullptr };
        std::shared_ptr<gui::Label> lblToHit { nullptr };
        std::shared_ptr<gui::Label> lblSlotName { nullptr };
        std::shared_ptr<gui::Button> btnBack { nullptr };
        std::shared_ptr<gui::Button> btnEquip{ nullptr };
        std::shared_ptr<gui::ListBox> lbDesc { nullptr };

        // KOTOR only
        std::shared_ptr<gui::Label> lblVitality { nullptr };
        std::shared_ptr<gui::Label> lblTxtBar { nullptr };
        std::shared_ptr<gui::Button> btnCharLeft { nullptr };
        std::shared_ptr<gui::Button> btnCharRight { nullptr };
        std::shared_ptr<gui::Label> lblSelectTitle { nullptr };
        std::shared_ptr<gui::Button> btnChange1 { nullptr };
        std::shared_ptr<gui::Button> btnChange2 { nullptr };
        // End KOTOR only

        // TSL only
        std::shared_ptr<gui::Label> lblBack1 { nullptr };
        std::shared_ptr<gui::Label> lblDefBack { nullptr };
        std::shared_ptr<gui::Label> lblBar[5];
        std::shared_ptr<gui::Label> lblAttackMod { nullptr };
        std::shared_ptr<gui::Label> lblDamText { nullptr };
        std::shared_ptr<gui::Button> btnSwapWeapons { nullptr };
        std::shared_ptr<gui::Button> btnPrevNpc { nullptr };
        std::shared_ptr<gui::Button> btnNextNpc { nullptr };
        std::shared_ptr<gui::Label> lblDefText { nullptr };
        // End TSL only
    } _binding;
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
    std::shared_ptr<graphics::Texture> getEmptySlotIcon(Slot slot) const;

    void bindControls();
};

} // namespace game

} // namespace reone
