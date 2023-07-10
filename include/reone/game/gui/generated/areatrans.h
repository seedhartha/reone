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

#include "reone/gui/control/label.h"
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_areatrans : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.LBL_DESCRIPTION = findControl<gui::Label>("LBL_DESCRIPTION");
        _controls.LBL_ICON = findControl<gui::Label>("LBL_ICON");
        _controls.LBL_TEXTBG = findControl<gui::Label>("LBL_TEXTBG");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Label> LBL_DESCRIPTION;
        std::shared_ptr<gui::Label> LBL_ICON;
        std::shared_ptr<gui::Label> LBL_TEXTBG;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
