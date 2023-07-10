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

class GUI_pazaakwager : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_LESS = findControl<gui::Button>("BTN_LESS");
        _controls.BTN_MORE = findControl<gui::Button>("BTN_MORE");
        _controls.BTN_QUIT = findControl<gui::Button>("BTN_QUIT");
        _controls.BTN_WAGER = findControl<gui::Button>("BTN_WAGER");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BG = findControl<gui::Label>("LBL_BG");
        _controls.LBL_MAXIMUM = findControl<gui::Label>("LBL_MAXIMUM");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
        _controls.LBL_WAGERVAL = findControl<gui::Label>("LBL_WAGERVAL");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_LESS;
        std::shared_ptr<gui::Button> BTN_MORE;
        std::shared_ptr<gui::Button> BTN_QUIT;
        std::shared_ptr<gui::Button> BTN_WAGER;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BG;
        std::shared_ptr<gui::Label> LBL_MAXIMUM;
        std::shared_ptr<gui::Label> LBL_TITLE;
        std::shared_ptr<gui::Label> LBL_WAGERVAL;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
