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
#include "reone/gui/control/imagebutton.h"
#include "reone/gui/control/label.h"
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_leveluppnl : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_STEPNAME1 = findControl<gui::Button>("BTN_STEPNAME1");
        _controls.BTN_STEPNAME2 = findControl<gui::Button>("BTN_STEPNAME2");
        _controls.BTN_STEPNAME3 = findControl<gui::Button>("BTN_STEPNAME3");
        _controls.BTN_STEPNAME4 = findControl<gui::Button>("BTN_STEPNAME4");
        _controls.BTN_STEPNAME5 = findControl<gui::Button>("BTN_STEPNAME5");
        _controls.LBL_1 = findControl<gui::ImageButton>("LBL_1");
        _controls.LBL_2 = findControl<gui::ImageButton>("LBL_2");
        _controls.LBL_3 = findControl<gui::ImageButton>("LBL_3");
        _controls.LBL_4 = findControl<gui::ImageButton>("LBL_4");
        _controls.LBL_5 = findControl<gui::ImageButton>("LBL_5");
        _controls.LBL_BG = findControl<gui::Label>("LBL_BG");
        _controls.LBL_NUM1 = findControl<gui::Label>("LBL_NUM1");
        _controls.LBL_NUM2 = findControl<gui::Label>("LBL_NUM2");
        _controls.LBL_NUM3 = findControl<gui::Label>("LBL_NUM3");
        _controls.LBL_NUM4 = findControl<gui::Label>("LBL_NUM4");
        _controls.LBL_NUM5 = findControl<gui::Label>("LBL_NUM5");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_STEPNAME1;
        std::shared_ptr<gui::Button> BTN_STEPNAME2;
        std::shared_ptr<gui::Button> BTN_STEPNAME3;
        std::shared_ptr<gui::Button> BTN_STEPNAME4;
        std::shared_ptr<gui::Button> BTN_STEPNAME5;
        std::shared_ptr<gui::ImageButton> LBL_1;
        std::shared_ptr<gui::ImageButton> LBL_2;
        std::shared_ptr<gui::ImageButton> LBL_3;
        std::shared_ptr<gui::ImageButton> LBL_4;
        std::shared_ptr<gui::ImageButton> LBL_5;
        std::shared_ptr<gui::Label> LBL_BG;
        std::shared_ptr<gui::Label> LBL_NUM1;
        std::shared_ptr<gui::Label> LBL_NUM2;
        std::shared_ptr<gui::Label> LBL_NUM3;
        std::shared_ptr<gui::Label> LBL_NUM4;
        std::shared_ptr<gui::Label> LBL_NUM5;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
