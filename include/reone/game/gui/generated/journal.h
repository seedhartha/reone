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

class GUI_journal : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_EXIT = findControl<gui::Button>("BTN_EXIT");
        _controls.BTN_FILTER_NAME = findControl<gui::Button>("BTN_FILTER_NAME");
        _controls.BTN_FILTER_PLANET = findControl<gui::Button>("BTN_FILTER_PLANET");
        _controls.BTN_FILTER_PRIORITY = findControl<gui::Button>("BTN_FILTER_PRIORITY");
        _controls.BTN_FILTER_TIME = findControl<gui::Button>("BTN_FILTER_TIME");
        _controls.BTN_MESSAGES = findControl<gui::Button>("BTN_MESSAGES");
        _controls.BTN_QUESTITEMS = findControl<gui::Button>("BTN_QUESTITEMS");
        _controls.BTN_SORT = findControl<gui::Button>("BTN_SORT");
        _controls.BTN_SWAPTEXT = findControl<gui::Button>("BTN_SWAPTEXT");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_ITEM_DESCRIPTION = findControl<gui::ListBox>("LBL_ITEM_DESCRIPTION");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
        _controls.LB_ITEMS = findControl<gui::ListBox>("LB_ITEMS");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_EXIT;
        std::shared_ptr<gui::Button> BTN_FILTER_NAME;
        std::shared_ptr<gui::Button> BTN_FILTER_PLANET;
        std::shared_ptr<gui::Button> BTN_FILTER_PRIORITY;
        std::shared_ptr<gui::Button> BTN_FILTER_TIME;
        std::shared_ptr<gui::Button> BTN_MESSAGES;
        std::shared_ptr<gui::Button> BTN_QUESTITEMS;
        std::shared_ptr<gui::Button> BTN_SORT;
        std::shared_ptr<gui::Button> BTN_SWAPTEXT;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::ListBox> LBL_ITEM_DESCRIPTION;
        std::shared_ptr<gui::Label> LBL_TITLE;
        std::shared_ptr<gui::ListBox> LB_ITEMS;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
