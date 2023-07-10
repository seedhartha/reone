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

class GUI_pwrlvlup : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.ACCEPT_BTN = findControl<gui::Button>("ACCEPT_BTN");
        _controls.BACK_BTN = findControl<gui::Button>("BACK_BTN");
        _controls.DESC_LBL = findControl<gui::Label>("DESC_LBL");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_POWER = findControl<gui::Label>("LBL_POWER");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
        _controls.LB_POWERS = findControl<gui::ListBox>("LB_POWERS");
        _controls.MAIN_TITLE_LBL = findControl<gui::Label>("MAIN_TITLE_LBL");
        _controls.RECOMMENDED_BTN = findControl<gui::Button>("RECOMMENDED_BTN");
        _controls.REMAINING_SELECTIONS_LBL = findControl<gui::Label>("REMAINING_SELECTIONS_LBL");
        _controls.SELECTIONS_REMAINING_LBL = findControl<gui::Label>("SELECTIONS_REMAINING_LBL");
        _controls.SELECT_BTN = findControl<gui::Button>("SELECT_BTN");
        _controls.SUB_TITLE_LBL = findControl<gui::Label>("SUB_TITLE_LBL");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> ACCEPT_BTN;
        std::shared_ptr<gui::Button> BACK_BTN;
        std::shared_ptr<gui::Label> DESC_LBL;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_POWER;
        std::shared_ptr<gui::ListBox> LB_DESC;
        std::shared_ptr<gui::ListBox> LB_POWERS;
        std::shared_ptr<gui::Label> MAIN_TITLE_LBL;
        std::shared_ptr<gui::Button> RECOMMENDED_BTN;
        std::shared_ptr<gui::Label> REMAINING_SELECTIONS_LBL;
        std::shared_ptr<gui::Label> SELECTIONS_REMAINING_LBL;
        std::shared_ptr<gui::Button> SELECT_BTN;
        std::shared_ptr<gui::Label> SUB_TITLE_LBL;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
