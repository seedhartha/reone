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

class GUI_gameover : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_LASTSAVE = findControl<gui::Button>("BTN_LASTSAVE");
        _controls.BTN_LOADGAME = findControl<gui::Button>("BTN_LOADGAME");
        _controls.BTN_QUIT = findControl<gui::Button>("BTN_QUIT");
        _controls.LBL_MESSAGE = findControl<gui::Label>("LBL_MESSAGE");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_LASTSAVE;
        std::shared_ptr<gui::Button> BTN_LOADGAME;
        std::shared_ptr<gui::Button> BTN_QUIT;
        std::shared_ptr<gui::Label> LBL_MESSAGE;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
