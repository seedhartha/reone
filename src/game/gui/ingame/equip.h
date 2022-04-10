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

#include "../../../gui/control/button.h"
#include "../../../gui/control/imagebutton.h"
#include "../../../gui/control/label.h"
#include "../../../gui/control/listbox.h"

#include "../../gui.h"

namespace reone {

namespace game {

class Creature;

}

namespace game {

class InGameMenu;

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

    Equipment(
        KotOR &game,
        InGameMenu &inGameMenu,
        GameServices &services);

    void load() override;

    void update();

private:
    static constexpr int kNumControlsBar = 5;

    struct Binding {
        std::shared_ptr<gui::Label> lblCantEquip;
        std::shared_ptr<gui::Label> lblAttackInfo;
        std::shared_ptr<gui::Label> lblToHitR;
        std::shared_ptr<gui::Label> lblPortBord;
        std::shared_ptr<gui::Label> lblPortrait;
        std::shared_ptr<gui::ListBox> lbItems;
        std::shared_ptr<gui::Label> lblDefInfo;
        std::unordered_map<Equipment::Slot, std::shared_ptr<gui::Label>> lblInv;
        std::unordered_map<Equipment::Slot, std::shared_ptr<gui::Button>> btnInv;
        std::shared_ptr<gui::Label> lblAtkL;
        std::shared_ptr<gui::Label> lblAtkR;
        std::shared_ptr<gui::Label> lblDef;
        std::shared_ptr<gui::Label> lblTitle;
        std::shared_ptr<gui::Label> lblDamage;
        std::shared_ptr<gui::Label> lblToHitL;
        std::shared_ptr<gui::Label> lblToHit;
        std::shared_ptr<gui::Label> lblSlotName;
        std::shared_ptr<gui::Button> btnBack;
        std::shared_ptr<gui::Button> btnEquip;
        std::shared_ptr<gui::ListBox> lbDesc;

        // KOTOR only
        std::shared_ptr<gui::Label> lblVitality;
        std::shared_ptr<gui::Label> lblTxtBar;
        std::shared_ptr<gui::Button> btnCharLeft;
        std::shared_ptr<gui::Button> btnCharRight;
        std::shared_ptr<gui::Label> lblSelectTitle;
        std::shared_ptr<gui::Button> btnChange1;
        std::shared_ptr<gui::Button> btnChange2;
        // END KOTOR only

        // TSL only
        std::shared_ptr<gui::Label> lblBack1;
        std::shared_ptr<gui::Label> lblDefBack;
        std::shared_ptr<gui::Label> lblBar[kNumControlsBar];
        std::shared_ptr<gui::Label> lblAttackMod;
        std::shared_ptr<gui::Label> lblDamText;
        std::shared_ptr<gui::Button> btnSwapWeapons;
        std::shared_ptr<gui::Button> btnPrevNpc;
        std::shared_ptr<gui::Button> btnNextNpc;
        std::shared_ptr<gui::Label> lblDefText;
        // END TSL only
    } _binding;

    InGameMenu &_inGameMenu;

    Slot _selectedSlot {Slot::None};
    int _selectedItemIdx {-1};

    void preloadControl(gui::Control &control) override;

    void bindControls();
    void configureItemsListBox();
    void updateEquipment();
    void updateItems();
    void updatePortraits();
    void selectSlot(Slot slot);

    std::shared_ptr<graphics::Texture> getItemFrameTexture(int stackSize) const;
    std::shared_ptr<graphics::Texture> getEmptySlotIcon(Slot slot) const;

    void onItemsListBoxItemClick(const std::string &item);
};

} // namespace game

} // namespace reone
