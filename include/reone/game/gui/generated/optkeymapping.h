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

class GUI_optkeymapping : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_Accept = findControl<gui::Button>("BTN_Accept");
        _controls.BTN_Cancel = findControl<gui::Button>("BTN_Cancel");
        _controls.BTN_Default = findControl<gui::Button>("BTN_Default");
        _controls.BTN_Filter_Game = findControl<gui::Button>("BTN_Filter_Game");
        _controls.BTN_Filter_Mini = findControl<gui::Button>("BTN_Filter_Mini");
        _controls.BTN_Filter_Move = findControl<gui::Button>("BTN_Filter_Move");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_Title = findControl<gui::Label>("LBL_Title");
        _controls.LST_EventList = findControl<gui::ListBox>("LST_EventList");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_Accept;
        std::shared_ptr<gui::Button> BTN_Cancel;
        std::shared_ptr<gui::Button> BTN_Default;
        std::shared_ptr<gui::Button> BTN_Filter_Game;
        std::shared_ptr<gui::Button> BTN_Filter_Mini;
        std::shared_ptr<gui::Button> BTN_Filter_Move;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_Title;
        std::shared_ptr<gui::ListBox> LST_EventList;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
