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
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_name : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_RANDOM = findControl<gui::Button>("BTN_RANDOM");
        _controls.END_BTN = findControl<gui::Button>("END_BTN");
        _controls.MAIN_TITLE_LBL = findControl<gui::Label>("MAIN_TITLE_LBL");
        _controls.NAME_BOX_EDIT = findControl<gui::Label>("NAME_BOX_EDIT");
        _controls.SUB_TITLE_LBL = findControl<gui::Label>("SUB_TITLE_LBL");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_RANDOM;
        std::shared_ptr<gui::Button> END_BTN;
        std::shared_ptr<gui::Label> MAIN_TITLE_LBL;
        std::shared_ptr<gui::Label> NAME_BOX_EDIT;
        std::shared_ptr<gui::Label> SUB_TITLE_LBL;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
