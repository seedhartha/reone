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

class GUI_ftchrgen : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_ACCEPT = findControl<gui::Button>("BTN_ACCEPT");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_RECOMMENDED = findControl<gui::Button>("BTN_RECOMMENDED");
        _controls.BTN_SELECT = findControl<gui::Button>("BTN_SELECT");
        _controls.DESC_LBL = findControl<gui::Label>("DESC_LBL");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_NAME = findControl<gui::Label>("LBL_NAME");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
        _controls.LB_FEATS = findControl<gui::ListBox>("LB_FEATS");
        _controls.MAIN_TITLE_LBL = findControl<gui::Label>("MAIN_TITLE_LBL");
        _controls.STD_REMAINING_SELECTIONS_LBL = findControl<gui::Label>("STD_REMAINING_SELECTIONS_LBL");
        _controls.STD_SELECTIONS_REMAINING_LBL = findControl<gui::Label>("STD_SELECTIONS_REMAINING_LBL");
        _controls.SUB_TITLE_LBL = findControl<gui::Label>("SUB_TITLE_LBL");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ACCEPT;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_RECOMMENDED;
        std::shared_ptr<gui::Button> BTN_SELECT;
        std::shared_ptr<gui::Label> DESC_LBL;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_NAME;
        std::shared_ptr<gui::ListBox> LB_DESC;
        std::shared_ptr<gui::ListBox> LB_FEATS;
        std::shared_ptr<gui::Label> MAIN_TITLE_LBL;
        std::shared_ptr<gui::Label> STD_REMAINING_SELECTIONS_LBL;
        std::shared_ptr<gui::Label> STD_SELECTIONS_REMAINING_LBL;
        std::shared_ptr<gui::Label> SUB_TITLE_LBL;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
