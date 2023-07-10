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
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_titlemusic : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_LOOP = findControl<gui::Button>("BTN_LOOP");
        _controls.BTN_NEXT = findControl<gui::Button>("BTN_NEXT");
        _controls.BTN_PLAY = findControl<gui::Button>("BTN_PLAY");
        _controls.BTN_STOP = findControl<gui::Button>("BTN_STOP");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_MUSIC_TITLE = findControl<gui::Label>("LBL_MUSIC_TITLE");
        _controls.LBL_TRACKNAME = findControl<gui::Label>("LBL_TRACKNAME");
        _controls.LBL_TRACKNUM = findControl<gui::Label>("LBL_TRACKNUM");
        _controls.LBL_UNLOCKED = findControl<gui::Label>("LBL_UNLOCKED");
        _controls.LB_MUSIC = findControl<gui::ListBox>("LB_MUSIC");
        _controls.SLI_VOLUME = findControl<gui::Slider>("SLI_VOLUME");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_LOOP;
        std::shared_ptr<gui::Button> BTN_NEXT;
        std::shared_ptr<gui::Button> BTN_PLAY;
        std::shared_ptr<gui::Button> BTN_STOP;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_MUSIC_TITLE;
        std::shared_ptr<gui::Label> LBL_TRACKNAME;
        std::shared_ptr<gui::Label> LBL_TRACKNUM;
        std::shared_ptr<gui::Label> LBL_UNLOCKED;
        std::shared_ptr<gui::ListBox> LB_MUSIC;
        std::shared_ptr<gui::Slider> SLI_VOLUME;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
