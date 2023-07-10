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

class GUI_abchrgen : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_ACCEPT = findControl<gui::Button>("BTN_ACCEPT");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_RECOMMENDED = findControl<gui::Button>("BTN_RECOMMENDED");
        _controls.CHA_LBL = findControl<gui::Label>("CHA_LBL");
        _controls.CHA_MINUS_BTN = findControl<gui::Button>("CHA_MINUS_BTN");
        _controls.CHA_PLUS_BTN = findControl<gui::Button>("CHA_PLUS_BTN");
        _controls.CHA_POINTS_BTN = findControl<gui::Button>("CHA_POINTS_BTN");
        _controls.CON_LBL = findControl<gui::Label>("CON_LBL");
        _controls.CON_MINUS_BTN = findControl<gui::Button>("CON_MINUS_BTN");
        _controls.CON_PLUS_BTN = findControl<gui::Button>("CON_PLUS_BTN");
        _controls.CON_POINTS_BTN = findControl<gui::Button>("CON_POINTS_BTN");
        _controls.COST_LBL = findControl<gui::Label>("COST_LBL");
        _controls.COST_POINTS_LBL = findControl<gui::Label>("COST_POINTS_LBL");
        _controls.DESC_LBL = findControl<gui::Label>("DESC_LBL");
        _controls.DEX_LBL = findControl<gui::Label>("DEX_LBL");
        _controls.DEX_MINUS_BTN = findControl<gui::Button>("DEX_MINUS_BTN");
        _controls.DEX_PLUS_BTN = findControl<gui::Button>("DEX_PLUS_BTN");
        _controls.DEX_POINTS_BTN = findControl<gui::Button>("DEX_POINTS_BTN");
        _controls.INT_LBL = findControl<gui::Label>("INT_LBL");
        _controls.INT_MINUS_BTN = findControl<gui::Button>("INT_MINUS_BTN");
        _controls.INT_PLUS_BTN = findControl<gui::Button>("INT_PLUS_BTN");
        _controls.INT_POINTS_BTN = findControl<gui::Button>("INT_POINTS_BTN");
        _controls.LBL_ABILITY_MOD = findControl<gui::Label>("LBL_ABILITY_MOD");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BONUS_CHA = findControl<gui::Label>("LBL_BONUS_CHA");
        _controls.LBL_BONUS_CON = findControl<gui::Label>("LBL_BONUS_CON");
        _controls.LBL_BONUS_DEX = findControl<gui::Label>("LBL_BONUS_DEX");
        _controls.LBL_BONUS_INT = findControl<gui::Label>("LBL_BONUS_INT");
        _controls.LBL_BONUS_STR = findControl<gui::Label>("LBL_BONUS_STR");
        _controls.LBL_BONUS_WIS = findControl<gui::Label>("LBL_BONUS_WIS");
        _controls.LBL_MODIFIER = findControl<gui::Label>("LBL_MODIFIER");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
        _controls.MAIN_TITLE_LBL = findControl<gui::Label>("MAIN_TITLE_LBL");
        _controls.REMAINING_SELECTIONS_LBL = findControl<gui::Label>("REMAINING_SELECTIONS_LBL");
        _controls.SELECTIONS_REMAINING_LBL = findControl<gui::Label>("SELECTIONS_REMAINING_LBL");
        _controls.STR_LBL = findControl<gui::Label>("STR_LBL");
        _controls.STR_MINUS_BTN = findControl<gui::Button>("STR_MINUS_BTN");
        _controls.STR_PLUS_BTN = findControl<gui::Button>("STR_PLUS_BTN");
        _controls.STR_POINTS_BTN = findControl<gui::Button>("STR_POINTS_BTN");
        _controls.SUB_TITLE_LBL = findControl<gui::Label>("SUB_TITLE_LBL");
        _controls.WIS_LBL = findControl<gui::Label>("WIS_LBL");
        _controls.WIS_MINUS_BTN = findControl<gui::Button>("WIS_MINUS_BTN");
        _controls.WIS_PLUS_BTN = findControl<gui::Button>("WIS_PLUS_BTN");
        _controls.WIS_POINTS_BTN = findControl<gui::Button>("WIS_POINTS_BTN");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ACCEPT;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_RECOMMENDED;
        std::shared_ptr<gui::Label> CHA_LBL;
        std::shared_ptr<gui::Button> CHA_MINUS_BTN;
        std::shared_ptr<gui::Button> CHA_PLUS_BTN;
        std::shared_ptr<gui::Button> CHA_POINTS_BTN;
        std::shared_ptr<gui::Label> CON_LBL;
        std::shared_ptr<gui::Button> CON_MINUS_BTN;
        std::shared_ptr<gui::Button> CON_PLUS_BTN;
        std::shared_ptr<gui::Button> CON_POINTS_BTN;
        std::shared_ptr<gui::Label> COST_LBL;
        std::shared_ptr<gui::Label> COST_POINTS_LBL;
        std::shared_ptr<gui::Label> DESC_LBL;
        std::shared_ptr<gui::Label> DEX_LBL;
        std::shared_ptr<gui::Button> DEX_MINUS_BTN;
        std::shared_ptr<gui::Button> DEX_PLUS_BTN;
        std::shared_ptr<gui::Button> DEX_POINTS_BTN;
        std::shared_ptr<gui::Label> INT_LBL;
        std::shared_ptr<gui::Button> INT_MINUS_BTN;
        std::shared_ptr<gui::Button> INT_PLUS_BTN;
        std::shared_ptr<gui::Button> INT_POINTS_BTN;
        std::shared_ptr<gui::Label> LBL_ABILITY_MOD;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BONUS_CHA;
        std::shared_ptr<gui::Label> LBL_BONUS_CON;
        std::shared_ptr<gui::Label> LBL_BONUS_DEX;
        std::shared_ptr<gui::Label> LBL_BONUS_INT;
        std::shared_ptr<gui::Label> LBL_BONUS_STR;
        std::shared_ptr<gui::Label> LBL_BONUS_WIS;
        std::shared_ptr<gui::Label> LBL_MODIFIER;
        std::shared_ptr<gui::ListBox> LB_DESC;
        std::shared_ptr<gui::Label> MAIN_TITLE_LBL;
        std::shared_ptr<gui::Label> REMAINING_SELECTIONS_LBL;
        std::shared_ptr<gui::Label> SELECTIONS_REMAINING_LBL;
        std::shared_ptr<gui::Label> STR_LBL;
        std::shared_ptr<gui::Button> STR_MINUS_BTN;
        std::shared_ptr<gui::Button> STR_PLUS_BTN;
        std::shared_ptr<gui::Button> STR_POINTS_BTN;
        std::shared_ptr<gui::Label> SUB_TITLE_LBL;
        std::shared_ptr<gui::Label> WIS_LBL;
        std::shared_ptr<gui::Button> WIS_MINUS_BTN;
        std::shared_ptr<gui::Button> WIS_PLUS_BTN;
        std::shared_ptr<gui::Button> WIS_POINTS_BTN;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
