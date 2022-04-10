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

#include "../../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;
class ListBox;

} // namespace gui

namespace game {

class InventoryMenu : public GameGUI {
public:
    InventoryMenu(Game &game, GameServices &services);

    void load() override;

    void refreshPortraits();

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnExit;
        std::shared_ptr<gui::Button> btnUseItem;
        std::shared_ptr<gui::Label> lblCredits;
        std::shared_ptr<gui::Label> lblCreditsValue;
        std::shared_ptr<gui::Label> lblInv;
        std::shared_ptr<gui::ListBox> lbDescription;
        std::shared_ptr<gui::ListBox> lbItems;

        // KotOR only
        std::shared_ptr<gui::Button> btnChange1;
        std::shared_ptr<gui::Button> btnChange2;
        std::shared_ptr<gui::Button> btnQuestItems;
        std::shared_ptr<gui::Label> lblBgPort;
        std::shared_ptr<gui::Label> lblBgStats;
        std::shared_ptr<gui::Label> lblDef;
        std::shared_ptr<gui::Label> lblPort;
        std::shared_ptr<gui::Label> lblVit;
        // END KotOR only

        // TSL only
        std::shared_ptr<gui::Button> btnAll;
        std::shared_ptr<gui::Button> btnArmor;
        std::shared_ptr<gui::Button> btnDatapads;
        std::shared_ptr<gui::Button> btnMisc;
        std::shared_ptr<gui::Button> btnQuests;
        std::shared_ptr<gui::Button> btnUseable;
        std::shared_ptr<gui::Button> btnWeapons;
        std::shared_ptr<gui::Label> lblBar1;
        std::shared_ptr<gui::Label> lblBar2;
        std::shared_ptr<gui::Label> lblBar3;
        std::shared_ptr<gui::Label> lblBar4;
        std::shared_ptr<gui::Label> lblBar5;
        std::shared_ptr<gui::Label> lblBar6;
        std::shared_ptr<gui::Label> lblFilter;
        // END TSL only
    } _binding;

    void bindControls();
};

} // namespace game

} // namespace reone
