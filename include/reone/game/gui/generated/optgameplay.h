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
#include "reone/gui/control/togglebutton.h"
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_optgameplay : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_DEFAULT = findControl<gui::Button>("BTN_DEFAULT");
        _controls.BTN_DIFFICULTY = findControl<gui::Button>("BTN_DIFFICULTY");
        _controls.BTN_DIFFLEFT = findControl<gui::Button>("BTN_DIFFLEFT");
        _controls.BTN_DIFFRIGHT = findControl<gui::Button>("BTN_DIFFRIGHT");
        _controls.BTN_KEYMAP = findControl<gui::Button>("BTN_KEYMAP");
        _controls.BTN_MOUSE = findControl<gui::Button>("BTN_MOUSE");
        _controls.CB_AUTOSAVE = findControl<gui::ToggleButton>("CB_AUTOSAVE");
        _controls.CB_DISABLEMOVE = findControl<gui::ToggleButton>("CB_DISABLEMOVE");
        _controls.CB_INVERTCAM = findControl<gui::ToggleButton>("CB_INVERTCAM");
        _controls.CB_LEVELUP = findControl<gui::ToggleButton>("CB_LEVELUP");
        _controls.CB_REVERSE = findControl<gui::ToggleButton>("CB_REVERSE");
        _controls.CB_REVERSE_INGAME = findControl<gui::ToggleButton>("CB_REVERSE_INGAME");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_DEFAULT;
        std::shared_ptr<gui::Button> BTN_DIFFICULTY;
        std::shared_ptr<gui::Button> BTN_DIFFLEFT;
        std::shared_ptr<gui::Button> BTN_DIFFRIGHT;
        std::shared_ptr<gui::Button> BTN_KEYMAP;
        std::shared_ptr<gui::Button> BTN_MOUSE;
        std::shared_ptr<gui::ToggleButton> CB_AUTOSAVE;
        std::shared_ptr<gui::ToggleButton> CB_DISABLEMOVE;
        std::shared_ptr<gui::ToggleButton> CB_INVERTCAM;
        std::shared_ptr<gui::ToggleButton> CB_LEVELUP;
        std::shared_ptr<gui::ToggleButton> CB_REVERSE;
        std::shared_ptr<gui::ToggleButton> CB_REVERSE_INGAME;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_TITLE;
        std::shared_ptr<gui::ListBox> LB_DESC;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
