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
#include "reone/gui/control/slider.h"
#include "reone/gui/control/togglebutton.h"
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_optgraphics : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_ADVANCED = findControl<gui::Button>("BTN_ADVANCED");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_DEFAULT = findControl<gui::Button>("BTN_DEFAULT");
        _controls.BTN_RESOLUTION = findControl<gui::Button>("BTN_RESOLUTION");
        _controls.CB_FORCESPEED = findControl<gui::ToggleButton>("CB_FORCESPEED");
        _controls.CB_GRASS = findControl<gui::ToggleButton>("CB_GRASS");
        _controls.CB_SHADOWS = findControl<gui::ToggleButton>("CB_SHADOWS");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_GAMMA = findControl<gui::Label>("LBL_GAMMA");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
        _controls.SLI_GAMMA = findControl<gui::Slider>("SLI_GAMMA");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ADVANCED;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_DEFAULT;
        std::shared_ptr<gui::Button> BTN_RESOLUTION;
        std::shared_ptr<gui::ToggleButton> CB_FORCESPEED;
        std::shared_ptr<gui::ToggleButton> CB_GRASS;
        std::shared_ptr<gui::ToggleButton> CB_SHADOWS;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_GAMMA;
        std::shared_ptr<gui::Label> LBL_TITLE;
        std::shared_ptr<gui::ListBox> LB_DESC;
        std::shared_ptr<gui::Slider> SLI_GAMMA;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
