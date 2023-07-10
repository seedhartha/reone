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

class GUI_optsoundadv : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_CANCEL = findControl<gui::Button>("BTN_CANCEL");
        _controls.BTN_DEFAULT = findControl<gui::Button>("BTN_DEFAULT");
        _controls.BTN_EAX = findControl<gui::Button>("BTN_EAX");
        _controls.BTN_EAXLEFT = findControl<gui::Button>("BTN_EAXLEFT");
        _controls.BTN_EAXRIGHT = findControl<gui::Button>("BTN_EAXRIGHT");
        _controls.CB_FORCESOFTWARE = findControl<gui::ToggleButton>("CB_FORCESOFTWARE");
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
        std::shared_ptr<gui::Button> BTN_CANCEL;
        std::shared_ptr<gui::Button> BTN_DEFAULT;
        std::shared_ptr<gui::Button> BTN_EAX;
        std::shared_ptr<gui::Button> BTN_EAXLEFT;
        std::shared_ptr<gui::Button> BTN_EAXRIGHT;
        std::shared_ptr<gui::ToggleButton> CB_FORCESOFTWARE;
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
