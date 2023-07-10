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

class GUI_skchrgen : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.AWARENESS_LBL = findControl<gui::Label>("AWARENESS_LBL");
        _controls.AWARENESS_POINTS_BTN = findControl<gui::Button>("AWARENESS_POINTS_BTN");
        _controls.AWA_MINUS_BTN = findControl<gui::Button>("AWA_MINUS_BTN");
        _controls.AWA_PLUS_BTN = findControl<gui::Button>("AWA_PLUS_BTN");
        _controls.BTN_ACCEPT = findControl<gui::Button>("BTN_ACCEPT");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_RECOMMENDED = findControl<gui::Button>("BTN_RECOMMENDED");
        _controls.CLASSSKL_LBL = findControl<gui::Label>("CLASSSKL_LBL");
        _controls.COMPUTER_USE_LBL = findControl<gui::Label>("COMPUTER_USE_LBL");
        _controls.COMPUTER_USE_POINTS_BTN = findControl<gui::Button>("COMPUTER_USE_POINTS_BTN");
        _controls.COM_MINUS_BTN = findControl<gui::Button>("COM_MINUS_BTN");
        _controls.COM_PLUS_BTN = findControl<gui::Button>("COM_PLUS_BTN");
        _controls.COST_LBL = findControl<gui::Label>("COST_LBL");
        _controls.COST_POINTS_LBL = findControl<gui::Label>("COST_POINTS_LBL");
        _controls.DEMOLITIONS_LBL = findControl<gui::Label>("DEMOLITIONS_LBL");
        _controls.DEMOLITIONS_POINTS_BTN = findControl<gui::Button>("DEMOLITIONS_POINTS_BTN");
        _controls.DEM_MINUS_BTN = findControl<gui::Button>("DEM_MINUS_BTN");
        _controls.DEM_PLUS_BTN = findControl<gui::Button>("DEM_PLUS_BTN");
        _controls.DESC_LBL = findControl<gui::Label>("DESC_LBL");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
        _controls.MAIN_TITLE_LBL = findControl<gui::Label>("MAIN_TITLE_LBL");
        _controls.PERSUADE_LBL = findControl<gui::Label>("PERSUADE_LBL");
        _controls.PERSUADE_POINTS_BTN = findControl<gui::Button>("PERSUADE_POINTS_BTN");
        _controls.PER_MINUS_BTN = findControl<gui::Button>("PER_MINUS_BTN");
        _controls.PER_PLUS_BTN = findControl<gui::Button>("PER_PLUS_BTN");
        _controls.REMAINING_SELECTIONS_LBL = findControl<gui::Label>("REMAINING_SELECTIONS_LBL");
        _controls.REPAIR_LBL = findControl<gui::Label>("REPAIR_LBL");
        _controls.REPAIR_POINTS_BTN = findControl<gui::Button>("REPAIR_POINTS_BTN");
        _controls.REP_MINUS_BTN = findControl<gui::Button>("REP_MINUS_BTN");
        _controls.REP_PLUS_BTN = findControl<gui::Button>("REP_PLUS_BTN");
        _controls.SECURITY_LBL = findControl<gui::Label>("SECURITY_LBL");
        _controls.SECURITY_POINTS_BTN = findControl<gui::Button>("SECURITY_POINTS_BTN");
        _controls.SEC_MINUS_BTN = findControl<gui::Button>("SEC_MINUS_BTN");
        _controls.SEC_PLUS_BTN = findControl<gui::Button>("SEC_PLUS_BTN");
        _controls.SELECTIONS_REMAINING_LBL = findControl<gui::Label>("SELECTIONS_REMAINING_LBL");
        _controls.STEALTH_LBL = findControl<gui::Label>("STEALTH_LBL");
        _controls.STEALTH_POINTS_BTN = findControl<gui::Button>("STEALTH_POINTS_BTN");
        _controls.STE_MINUS_BTN = findControl<gui::Button>("STE_MINUS_BTN");
        _controls.STE_PLUS_BTN = findControl<gui::Button>("STE_PLUS_BTN");
        _controls.SUB_TITLE_LBL = findControl<gui::Label>("SUB_TITLE_LBL");
        _controls.TREAT_INJURY_LBL = findControl<gui::Label>("TREAT_INJURY_LBL");
        _controls.TREAT_INJURY_POINTS_BTN = findControl<gui::Button>("TREAT_INJURY_POINTS_BTN");
        _controls.TRE_MINUS_BTN = findControl<gui::Button>("TRE_MINUS_BTN");
        _controls.TRE_PLUS_BTN = findControl<gui::Button>("TRE_PLUS_BTN");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Label> AWARENESS_LBL;
        std::shared_ptr<gui::Button> AWARENESS_POINTS_BTN;
        std::shared_ptr<gui::Button> AWA_MINUS_BTN;
        std::shared_ptr<gui::Button> AWA_PLUS_BTN;
        std::shared_ptr<gui::Button> BTN_ACCEPT;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_RECOMMENDED;
        std::shared_ptr<gui::Label> CLASSSKL_LBL;
        std::shared_ptr<gui::Label> COMPUTER_USE_LBL;
        std::shared_ptr<gui::Button> COMPUTER_USE_POINTS_BTN;
        std::shared_ptr<gui::Button> COM_MINUS_BTN;
        std::shared_ptr<gui::Button> COM_PLUS_BTN;
        std::shared_ptr<gui::Label> COST_LBL;
        std::shared_ptr<gui::Label> COST_POINTS_LBL;
        std::shared_ptr<gui::Label> DEMOLITIONS_LBL;
        std::shared_ptr<gui::Button> DEMOLITIONS_POINTS_BTN;
        std::shared_ptr<gui::Button> DEM_MINUS_BTN;
        std::shared_ptr<gui::Button> DEM_PLUS_BTN;
        std::shared_ptr<gui::Label> DESC_LBL;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::ListBox> LB_DESC;
        std::shared_ptr<gui::Label> MAIN_TITLE_LBL;
        std::shared_ptr<gui::Label> PERSUADE_LBL;
        std::shared_ptr<gui::Button> PERSUADE_POINTS_BTN;
        std::shared_ptr<gui::Button> PER_MINUS_BTN;
        std::shared_ptr<gui::Button> PER_PLUS_BTN;
        std::shared_ptr<gui::Label> REMAINING_SELECTIONS_LBL;
        std::shared_ptr<gui::Label> REPAIR_LBL;
        std::shared_ptr<gui::Button> REPAIR_POINTS_BTN;
        std::shared_ptr<gui::Button> REP_MINUS_BTN;
        std::shared_ptr<gui::Button> REP_PLUS_BTN;
        std::shared_ptr<gui::Label> SECURITY_LBL;
        std::shared_ptr<gui::Button> SECURITY_POINTS_BTN;
        std::shared_ptr<gui::Button> SEC_MINUS_BTN;
        std::shared_ptr<gui::Button> SEC_PLUS_BTN;
        std::shared_ptr<gui::Label> SELECTIONS_REMAINING_LBL;
        std::shared_ptr<gui::Label> STEALTH_LBL;
        std::shared_ptr<gui::Button> STEALTH_POINTS_BTN;
        std::shared_ptr<gui::Button> STE_MINUS_BTN;
        std::shared_ptr<gui::Button> STE_PLUS_BTN;
        std::shared_ptr<gui::Label> SUB_TITLE_LBL;
        std::shared_ptr<gui::Label> TREAT_INJURY_LBL;
        std::shared_ptr<gui::Button> TREAT_INJURY_POINTS_BTN;
        std::shared_ptr<gui::Button> TRE_MINUS_BTN;
        std::shared_ptr<gui::Button> TRE_PLUS_BTN;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
