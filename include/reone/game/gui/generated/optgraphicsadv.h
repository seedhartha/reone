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

class GUI_optgraphicsadv : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_ANISOTROPY = findControl<gui::Button>("BTN_ANISOTROPY");
        _controls.BTN_ANISOTROPYLEFT = findControl<gui::Button>("BTN_ANISOTROPYLEFT");
        _controls.BTN_ANISOTROPYRIGHT = findControl<gui::Button>("BTN_ANISOTROPYRIGHT");
        _controls.BTN_ANTIALIAS = findControl<gui::Button>("BTN_ANTIALIAS");
        _controls.BTN_ANTIALIASLEFT = findControl<gui::Button>("BTN_ANTIALIASLEFT");
        _controls.BTN_ANTIALIASRIGHT = findControl<gui::Button>("BTN_ANTIALIASRIGHT");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_CANCEL = findControl<gui::Button>("BTN_CANCEL");
        _controls.BTN_DEFAULT = findControl<gui::Button>("BTN_DEFAULT");
        _controls.BTN_TEXQUAL = findControl<gui::Button>("BTN_TEXQUAL");
        _controls.BTN_TEXQUALLEFT = findControl<gui::Button>("BTN_TEXQUALLEFT");
        _controls.BTN_TEXQUALRIGHT = findControl<gui::Button>("BTN_TEXQUALRIGHT");
        _controls.CB_FRAMEBUFF = findControl<gui::ToggleButton>("CB_FRAMEBUFF");
        _controls.CB_SOFTSHADOWS = findControl<gui::ToggleButton>("CB_SOFTSHADOWS");
        _controls.CB_VSYNC = findControl<gui::ToggleButton>("CB_VSYNC");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ANISOTROPY;
        std::shared_ptr<gui::Button> BTN_ANISOTROPYLEFT;
        std::shared_ptr<gui::Button> BTN_ANISOTROPYRIGHT;
        std::shared_ptr<gui::Button> BTN_ANTIALIAS;
        std::shared_ptr<gui::Button> BTN_ANTIALIASLEFT;
        std::shared_ptr<gui::Button> BTN_ANTIALIASRIGHT;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_CANCEL;
        std::shared_ptr<gui::Button> BTN_DEFAULT;
        std::shared_ptr<gui::Button> BTN_TEXQUAL;
        std::shared_ptr<gui::Button> BTN_TEXQUALLEFT;
        std::shared_ptr<gui::Button> BTN_TEXQUALRIGHT;
        std::shared_ptr<gui::ToggleButton> CB_FRAMEBUFF;
        std::shared_ptr<gui::ToggleButton> CB_SOFTSHADOWS;
        std::shared_ptr<gui::ToggleButton> CB_VSYNC;
        std::shared_ptr<gui::Label> LBL_TITLE;
        std::shared_ptr<gui::ListBox> LB_DESC;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
