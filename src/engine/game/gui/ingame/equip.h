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

    struct Binding {
        gui::Label *lblCantEquip { nullptr };
        gui::Label *lblAttackInfo { nullptr };
        gui::Label *lblToHitR { nullptr };
        gui::Label *lblPortBord { nullptr };
        gui::Label *lblPortrait { nullptr };
        gui::ListBox *lbItems { nullptr };
        gui::Label *lblDefInfo { nullptr };
        std::unordered_map<Equipment::Slot, gui::Label *> lblInv;
        std::unordered_map<Equipment::Slot, gui::Button *> btnInv;
        gui::Label *lblAtkL { nullptr };
        gui::Label *lblAtkR { nullptr };
        gui::Label *lblDef { nullptr };
        gui::Label *lblTitle { nullptr };
        gui::Label *lblDamage { nullptr };
        gui::Label *lblToHitL { nullptr };
        gui::Label *lblToHit { nullptr };
        gui::Label *lblSlotName { nullptr };
        gui::Button *btnBack { nullptr };
        gui::Button *btnEquip{ nullptr };
        gui::ListBox *lbDesc { nullptr };

        // KOTOR only
        gui::Label *lblVitality { nullptr };
        gui::Label *lblTxtBar { nullptr };
        gui::Button *btnCharLeft { nullptr };
        gui::Button *btnCharRight { nullptr };
        gui::Label *lblSelectTitle { nullptr };
        gui::Button *btnChange1 { nullptr };
        gui::Button *btnChange2 { nullptr };
        // End KOTOR only

        // TSL only
        gui::Label *lblBack1 { nullptr };
        gui::Label *lblDefBack { nullptr };
        gui::Label *lblBar[5];
        gui::Label *lblAttackMod { nullptr };
        gui::Label *lblDamText { nullptr };
        gui::Button *btnSwapWeapons { nullptr };
        gui::Button *btnPrevNpc { nullptr };
        gui::Button *btnNextNpc { nullptr };
        gui::Label *lblDefText { nullptr };
        // End TSL only
    };
    Binding binding;
    void bindControls();
};

} // namespace game

} // namespace reone
