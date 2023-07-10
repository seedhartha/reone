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

class GUI_messages : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_COMBAT = findControl<gui::Button>("BTN_COMBAT");
        _controls.BTN_DIALOG = findControl<gui::Button>("BTN_DIALOG");
        _controls.BTN_EFFECTS = findControl<gui::Button>("BTN_EFFECTS");
        _controls.BTN_EXIT = findControl<gui::Button>("BTN_EXIT");
        _controls.BTN_FEEDBACK = findControl<gui::Button>("BTN_FEEDBACK");
        _controls.BTN_SHOW = findControl<gui::Button>("BTN_SHOW");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_BAR6 = findControl<gui::Label>("LBL_BAR6");
        _controls.LBL_EFFECTS_BAD = findControl<gui::Label>("LBL_EFFECTS_BAD");
        _controls.LBL_EFFECTS_GOOD = findControl<gui::Label>("LBL_EFFECTS_GOOD");
        _controls.LBL_FILTER = findControl<gui::Label>("LBL_FILTER");
        _controls.LBL_MESSAGES = findControl<gui::Label>("LBL_MESSAGES");
        _controls.LB_COMBAT = findControl<gui::ListBox>("LB_COMBAT");
        _controls.LB_DIALOG = findControl<gui::ListBox>("LB_DIALOG");
        _controls.LB_EFFECTS_BAD = findControl<gui::ListBox>("LB_EFFECTS_BAD");
        _controls.LB_EFFECTS_GOOD = findControl<gui::ListBox>("LB_EFFECTS_GOOD");
        _controls.LB_MESSAGES = findControl<gui::ListBox>("LB_MESSAGES");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_COMBAT;
        std::shared_ptr<gui::Button> BTN_DIALOG;
        std::shared_ptr<gui::Button> BTN_EFFECTS;
        std::shared_ptr<gui::Button> BTN_EXIT;
        std::shared_ptr<gui::Button> BTN_FEEDBACK;
        std::shared_ptr<gui::Button> BTN_SHOW;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_BAR6;
        std::shared_ptr<gui::Label> LBL_EFFECTS_BAD;
        std::shared_ptr<gui::Label> LBL_EFFECTS_GOOD;
        std::shared_ptr<gui::Label> LBL_FILTER;
        std::shared_ptr<gui::Label> LBL_MESSAGES;
        std::shared_ptr<gui::ListBox> LB_COMBAT;
        std::shared_ptr<gui::ListBox> LB_DIALOG;
        std::shared_ptr<gui::ListBox> LB_EFFECTS_BAD;
        std::shared_ptr<gui::ListBox> LB_EFFECTS_GOOD;
        std::shared_ptr<gui::ListBox> LB_MESSAGES;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
