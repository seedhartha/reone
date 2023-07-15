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
    InventoryMenu(Game &game, ServicesView &services) :
        GameGUI(game, services) {
        _resRef = guiResRef("inventory");
    }

    void refreshPortraits();

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ALL;
        std::shared_ptr<gui::Button> BTN_ARMOR;
        std::shared_ptr<gui::Button> BTN_CHANGE1;
        std::shared_ptr<gui::Button> BTN_CHANGE2;
        std::shared_ptr<gui::Button> BTN_DATAPADS;
        std::shared_ptr<gui::Button> BTN_EXIT;
        std::shared_ptr<gui::Button> BTN_MISC;
        std::shared_ptr<gui::Button> BTN_QUESTITEMS;
        std::shared_ptr<gui::Button> BTN_QUESTS;
        std::shared_ptr<gui::Button> BTN_USEABLE;
        std::shared_ptr<gui::Button> BTN_USEITEM;
        std::shared_ptr<gui::Button> BTN_WEAPONS;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_BAR6;
        std::shared_ptr<gui::Label> LBL_BGPORT;
        std::shared_ptr<gui::Label> LBL_BGSTATS;
        std::shared_ptr<gui::Label> LBL_CREDITS;
        std::shared_ptr<gui::Label> LBL_CREDITS_VALUE;
        std::shared_ptr<gui::Label> LBL_DEF;
        std::shared_ptr<gui::Label> LBL_FILTER;
        std::shared_ptr<gui::Label> LBL_INV;
        std::shared_ptr<gui::Label> LBL_PORT;
        std::shared_ptr<gui::Label> LBL_VIT;
        std::shared_ptr<gui::ListBox> LB_DESCRIPTION;
        std::shared_ptr<gui::ListBox> LB_ITEMS;
    };

    Controls _controls;

    void onGUILoaded() override;

    void bindControls() {
        _controls.BTN_ALL = findControl<gui::Button>("BTN_ALL");
        _controls.BTN_ARMOR = findControl<gui::Button>("BTN_ARMOR");
        _controls.BTN_CHANGE1 = findControl<gui::Button>("BTN_CHANGE1");
        _controls.BTN_CHANGE2 = findControl<gui::Button>("BTN_CHANGE2");
        _controls.BTN_DATAPADS = findControl<gui::Button>("BTN_DATAPADS");
        _controls.BTN_EXIT = findControl<gui::Button>("BTN_EXIT");
        _controls.BTN_MISC = findControl<gui::Button>("BTN_MISC");
        _controls.BTN_QUESTITEMS = findControl<gui::Button>("BTN_QUESTITEMS");
        _controls.BTN_QUESTS = findControl<gui::Button>("BTN_QUESTS");
        _controls.BTN_USEABLE = findControl<gui::Button>("BTN_USEABLE");
        _controls.BTN_USEITEM = findControl<gui::Button>("BTN_USEITEM");
        _controls.BTN_WEAPONS = findControl<gui::Button>("BTN_WEAPONS");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_BAR6 = findControl<gui::Label>("LBL_BAR6");
        _controls.LBL_BGPORT = findControl<gui::Label>("LBL_BGPORT");
        _controls.LBL_BGSTATS = findControl<gui::Label>("LBL_BGSTATS");
        _controls.LBL_CREDITS = findControl<gui::Label>("LBL_CREDITS");
        _controls.LBL_CREDITS_VALUE = findControl<gui::Label>("LBL_CREDITS_VALUE");
        _controls.LBL_DEF = findControl<gui::Label>("LBL_DEF");
        _controls.LBL_FILTER = findControl<gui::Label>("LBL_FILTER");
        _controls.LBL_INV = findControl<gui::Label>("LBL_INV");
        _controls.LBL_PORT = findControl<gui::Label>("LBL_PORT");
        _controls.LBL_VIT = findControl<gui::Label>("LBL_VIT");
        _controls.LB_DESCRIPTION = findControl<gui::ListBox>("LB_DESCRIPTION");
        _controls.LB_ITEMS = findControl<gui::ListBox>("LB_ITEMS");
    }
};

} // namespace game

} // namespace reone
