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
#include "reone/gui/control/togglebutton.h"
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_partyselect : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_ACCEPT = findControl<gui::Button>("BTN_ACCEPT");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_DONE = findControl<gui::Button>("BTN_DONE");
        _controls.BTN_NPC0 = findControl<gui::ToggleButton>("BTN_NPC0");
        _controls.BTN_NPC1 = findControl<gui::ToggleButton>("BTN_NPC1");
        _controls.BTN_NPC10 = findControl<gui::ToggleButton>("BTN_NPC10");
        _controls.BTN_NPC11 = findControl<gui::ToggleButton>("BTN_NPC11");
        _controls.BTN_NPC2 = findControl<gui::ToggleButton>("BTN_NPC2");
        _controls.BTN_NPC3 = findControl<gui::ToggleButton>("BTN_NPC3");
        _controls.BTN_NPC4 = findControl<gui::ToggleButton>("BTN_NPC4");
        _controls.BTN_NPC5 = findControl<gui::ToggleButton>("BTN_NPC5");
        _controls.BTN_NPC6 = findControl<gui::ToggleButton>("BTN_NPC6");
        _controls.BTN_NPC7 = findControl<gui::ToggleButton>("BTN_NPC7");
        _controls.BTN_NPC8 = findControl<gui::ToggleButton>("BTN_NPC8");
        _controls.BTN_NPC9 = findControl<gui::ToggleButton>("BTN_NPC9");
        _controls.LBL_3D = findControl<gui::Label>("LBL_3D");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_AVAILABLE = findControl<gui::Label>("LBL_AVAILABLE");
        _controls.LBL_BEVEL_L = findControl<gui::Label>("LBL_BEVEL_L");
        _controls.LBL_BEVEL_M = findControl<gui::Label>("LBL_BEVEL_M");
        _controls.LBL_BEVEL_R = findControl<gui::Label>("LBL_BEVEL_R");
        _controls.LBL_CHAR0 = findControl<gui::Label>("LBL_CHAR0");
        _controls.LBL_CHAR1 = findControl<gui::Label>("LBL_CHAR1");
        _controls.LBL_CHAR10 = findControl<gui::Label>("LBL_CHAR10");
        _controls.LBL_CHAR11 = findControl<gui::Label>("LBL_CHAR11");
        _controls.LBL_CHAR2 = findControl<gui::Label>("LBL_CHAR2");
        _controls.LBL_CHAR3 = findControl<gui::Label>("LBL_CHAR3");
        _controls.LBL_CHAR4 = findControl<gui::Label>("LBL_CHAR4");
        _controls.LBL_CHAR5 = findControl<gui::Label>("LBL_CHAR5");
        _controls.LBL_CHAR6 = findControl<gui::Label>("LBL_CHAR6");
        _controls.LBL_CHAR7 = findControl<gui::Label>("LBL_CHAR7");
        _controls.LBL_CHAR8 = findControl<gui::Label>("LBL_CHAR8");
        _controls.LBL_CHAR9 = findControl<gui::Label>("LBL_CHAR9");
        _controls.LBL_COUNT = findControl<gui::Label>("LBL_COUNT");
        _controls.LBL_NA0 = findControl<gui::Label>("LBL_NA0");
        _controls.LBL_NA1 = findControl<gui::Label>("LBL_NA1");
        _controls.LBL_NA10 = findControl<gui::Label>("LBL_NA10");
        _controls.LBL_NA11 = findControl<gui::Label>("LBL_NA11");
        _controls.LBL_NA2 = findControl<gui::Label>("LBL_NA2");
        _controls.LBL_NA3 = findControl<gui::Label>("LBL_NA3");
        _controls.LBL_NA4 = findControl<gui::Label>("LBL_NA4");
        _controls.LBL_NA5 = findControl<gui::Label>("LBL_NA5");
        _controls.LBL_NA6 = findControl<gui::Label>("LBL_NA6");
        _controls.LBL_NA7 = findControl<gui::Label>("LBL_NA7");
        _controls.LBL_NA8 = findControl<gui::Label>("LBL_NA8");
        _controls.LBL_NA9 = findControl<gui::Label>("LBL_NA9");
        _controls.LBL_NAMEBACK = findControl<gui::Label>("LBL_NAMEBACK");
        _controls.LBL_NPC_LEVEL = findControl<gui::Label>("LBL_NPC_LEVEL");
        _controls.LBL_NPC_NAME = findControl<gui::Label>("LBL_NPC_NAME");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ACCEPT;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_DONE;
        std::shared_ptr<gui::ToggleButton> BTN_NPC0;
        std::shared_ptr<gui::ToggleButton> BTN_NPC1;
        std::shared_ptr<gui::ToggleButton> BTN_NPC10;
        std::shared_ptr<gui::ToggleButton> BTN_NPC11;
        std::shared_ptr<gui::ToggleButton> BTN_NPC2;
        std::shared_ptr<gui::ToggleButton> BTN_NPC3;
        std::shared_ptr<gui::ToggleButton> BTN_NPC4;
        std::shared_ptr<gui::ToggleButton> BTN_NPC5;
        std::shared_ptr<gui::ToggleButton> BTN_NPC6;
        std::shared_ptr<gui::ToggleButton> BTN_NPC7;
        std::shared_ptr<gui::ToggleButton> BTN_NPC8;
        std::shared_ptr<gui::ToggleButton> BTN_NPC9;
        std::shared_ptr<gui::Label> LBL_3D;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_AVAILABLE;
        std::shared_ptr<gui::Label> LBL_BEVEL_L;
        std::shared_ptr<gui::Label> LBL_BEVEL_M;
        std::shared_ptr<gui::Label> LBL_BEVEL_R;
        std::shared_ptr<gui::Label> LBL_CHAR0;
        std::shared_ptr<gui::Label> LBL_CHAR1;
        std::shared_ptr<gui::Label> LBL_CHAR10;
        std::shared_ptr<gui::Label> LBL_CHAR11;
        std::shared_ptr<gui::Label> LBL_CHAR2;
        std::shared_ptr<gui::Label> LBL_CHAR3;
        std::shared_ptr<gui::Label> LBL_CHAR4;
        std::shared_ptr<gui::Label> LBL_CHAR5;
        std::shared_ptr<gui::Label> LBL_CHAR6;
        std::shared_ptr<gui::Label> LBL_CHAR7;
        std::shared_ptr<gui::Label> LBL_CHAR8;
        std::shared_ptr<gui::Label> LBL_CHAR9;
        std::shared_ptr<gui::Label> LBL_COUNT;
        std::shared_ptr<gui::Label> LBL_NA0;
        std::shared_ptr<gui::Label> LBL_NA1;
        std::shared_ptr<gui::Label> LBL_NA10;
        std::shared_ptr<gui::Label> LBL_NA11;
        std::shared_ptr<gui::Label> LBL_NA2;
        std::shared_ptr<gui::Label> LBL_NA3;
        std::shared_ptr<gui::Label> LBL_NA4;
        std::shared_ptr<gui::Label> LBL_NA5;
        std::shared_ptr<gui::Label> LBL_NA6;
        std::shared_ptr<gui::Label> LBL_NA7;
        std::shared_ptr<gui::Label> LBL_NA8;
        std::shared_ptr<gui::Label> LBL_NA9;
        std::shared_ptr<gui::Label> LBL_NAMEBACK;
        std::shared_ptr<gui::Label> LBL_NPC_LEVEL;
        std::shared_ptr<gui::Label> LBL_NPC_NAME;
        std::shared_ptr<gui::Label> LBL_TITLE;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
