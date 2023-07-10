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

class GUI_mainmenu8x6 : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_EXIT = findControl<gui::Button>("BTN_EXIT");
        _controls.BTN_LOADGAME = findControl<gui::Button>("BTN_LOADGAME");
        _controls.BTN_MOREGAMES = findControl<gui::Button>("BTN_MOREGAMES");
        _controls.BTN_MOVIES = findControl<gui::Button>("BTN_MOVIES");
        _controls.BTN_MUSIC = findControl<gui::Button>("BTN_MUSIC");
        _controls.BTN_NEWGAME = findControl<gui::Button>("BTN_NEWGAME");
        _controls.BTN_OPTIONS = findControl<gui::Button>("BTN_OPTIONS");
        _controls.BTN_TSLRCM = findControl<gui::Button>("BTN_TSLRCM");
        _controls.BTN_WARP = findControl<gui::Button>("BTN_WARP");
        _controls.LBL_3DVIEW = findControl<gui::Label>("LBL_3DVIEW");
        _controls.LBL_BW = findControl<gui::Label>("LBL_BW");
        _controls.LBL_GAMELOGO = findControl<gui::Label>("LBL_GAMELOGO");
        _controls.LBL_LUCAS = findControl<gui::Label>("LBL_LUCAS");
        _controls.LBL_MENUBG = findControl<gui::Label>("LBL_MENUBG");
        _controls.LBL_NEWCONTENT = findControl<gui::Label>("LBL_NEWCONTENT");
        _controls.LB_MODULES = findControl<gui::ListBox>("LB_MODULES");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_EXIT;
        std::shared_ptr<gui::Button> BTN_LOADGAME;
        std::shared_ptr<gui::Button> BTN_MOREGAMES;
        std::shared_ptr<gui::Button> BTN_MOVIES;
        std::shared_ptr<gui::Button> BTN_MUSIC;
        std::shared_ptr<gui::Button> BTN_NEWGAME;
        std::shared_ptr<gui::Button> BTN_OPTIONS;
        std::shared_ptr<gui::Button> BTN_TSLRCM;
        std::shared_ptr<gui::Button> BTN_WARP;
        std::shared_ptr<gui::Label> LBL_3DVIEW;
        std::shared_ptr<gui::Label> LBL_BW;
        std::shared_ptr<gui::Label> LBL_GAMELOGO;
        std::shared_ptr<gui::Label> LBL_LUCAS;
        std::shared_ptr<gui::Label> LBL_MENUBG;
        std::shared_ptr<gui::Label> LBL_NEWCONTENT;
        std::shared_ptr<gui::ListBox> LB_MODULES;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
