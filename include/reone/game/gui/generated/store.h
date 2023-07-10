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

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_store : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_ALL = findControl<gui::Button>("BTN_ALL");
        _controls.BTN_ARMOR = findControl<gui::Button>("BTN_ARMOR");
        _controls.BTN_Accept = findControl<gui::Button>("BTN_Accept");
        _controls.BTN_Cancel = findControl<gui::Button>("BTN_Cancel");
        _controls.BTN_Examine = findControl<gui::Button>("BTN_Examine");
        _controls.BTN_MISC = findControl<gui::Button>("BTN_MISC");
        _controls.BTN_WEAPONS = findControl<gui::Button>("BTN_WEAPONS");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_BUYSELL = findControl<gui::Label>("LBL_BUYSELL");
        _controls.LBL_COST = findControl<gui::Label>("LBL_COST");
        _controls.LBL_COST_VALUE = findControl<gui::Label>("LBL_COST_VALUE");
        _controls.LBL_CREDITS = findControl<gui::Label>("LBL_CREDITS");
        _controls.LBL_CREDITS_VALUE = findControl<gui::Label>("LBL_CREDITS_VALUE");
        _controls.LBL_FILTER = findControl<gui::Label>("LBL_FILTER");
        _controls.LBL_ONHAND = findControl<gui::Label>("LBL_ONHAND");
        _controls.LBL_ONHAND_VALUE = findControl<gui::Label>("LBL_ONHAND_VALUE");
        _controls.LBL_STOCK = findControl<gui::Label>("LBL_STOCK");
        _controls.LBL_STOCK_VALUE = findControl<gui::Label>("LBL_STOCK_VALUE");
        _controls.LB_DESCRIPTION = findControl<gui::ListBox>("LB_DESCRIPTION");
        _controls.LB_INVITEMS = findControl<gui::ListBox>("LB_INVITEMS");
        _controls.LB_SHOPITEMS = findControl<gui::ListBox>("LB_SHOPITEMS");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ALL;
        std::shared_ptr<gui::Button> BTN_ARMOR;
        std::shared_ptr<gui::Button> BTN_Accept;
        std::shared_ptr<gui::Button> BTN_Cancel;
        std::shared_ptr<gui::Button> BTN_Examine;
        std::shared_ptr<gui::Button> BTN_MISC;
        std::shared_ptr<gui::Button> BTN_WEAPONS;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_BUYSELL;
        std::shared_ptr<gui::Label> LBL_COST;
        std::shared_ptr<gui::Label> LBL_COST_VALUE;
        std::shared_ptr<gui::Label> LBL_CREDITS;
        std::shared_ptr<gui::Label> LBL_CREDITS_VALUE;
        std::shared_ptr<gui::Label> LBL_FILTER;
        std::shared_ptr<gui::Label> LBL_ONHAND;
        std::shared_ptr<gui::Label> LBL_ONHAND_VALUE;
        std::shared_ptr<gui::Label> LBL_STOCK;
        std::shared_ptr<gui::Label> LBL_STOCK_VALUE;
        std::shared_ptr<gui::ListBox> LB_DESCRIPTION;
        std::shared_ptr<gui::ListBox> LB_INVITEMS;
        std::shared_ptr<gui::ListBox> LB_SHOPITEMS;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
