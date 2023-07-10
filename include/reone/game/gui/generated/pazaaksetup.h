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

class GUI_pazaaksetup : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_ATEXT = findControl<gui::Button>("BTN_ATEXT");
        _controls.BTN_AVAIL00 = findControl<gui::Button>("BTN_AVAIL00");
        _controls.BTN_AVAIL01 = findControl<gui::Button>("BTN_AVAIL01");
        _controls.BTN_AVAIL02 = findControl<gui::Button>("BTN_AVAIL02");
        _controls.BTN_AVAIL03 = findControl<gui::Button>("BTN_AVAIL03");
        _controls.BTN_AVAIL04 = findControl<gui::Button>("BTN_AVAIL04");
        _controls.BTN_AVAIL05 = findControl<gui::Button>("BTN_AVAIL05");
        _controls.BTN_AVAIL10 = findControl<gui::Button>("BTN_AVAIL10");
        _controls.BTN_AVAIL11 = findControl<gui::Button>("BTN_AVAIL11");
        _controls.BTN_AVAIL12 = findControl<gui::Button>("BTN_AVAIL12");
        _controls.BTN_AVAIL13 = findControl<gui::Button>("BTN_AVAIL13");
        _controls.BTN_AVAIL14 = findControl<gui::Button>("BTN_AVAIL14");
        _controls.BTN_AVAIL15 = findControl<gui::Button>("BTN_AVAIL15");
        _controls.BTN_AVAIL20 = findControl<gui::Button>("BTN_AVAIL20");
        _controls.BTN_AVAIL21 = findControl<gui::Button>("BTN_AVAIL21");
        _controls.BTN_AVAIL22 = findControl<gui::Button>("BTN_AVAIL22");
        _controls.BTN_AVAIL23 = findControl<gui::Button>("BTN_AVAIL23");
        _controls.BTN_AVAIL24 = findControl<gui::Button>("BTN_AVAIL24");
        _controls.BTN_AVAIL25 = findControl<gui::Button>("BTN_AVAIL25");
        _controls.BTN_AVAIL30 = findControl<gui::Button>("BTN_AVAIL30");
        _controls.BTN_AVAIL31 = findControl<gui::Button>("BTN_AVAIL31");
        _controls.BTN_AVAIL32 = findControl<gui::Button>("BTN_AVAIL32");
        _controls.BTN_AVAIL33 = findControl<gui::Button>("BTN_AVAIL33");
        _controls.BTN_AVAIL34 = findControl<gui::Button>("BTN_AVAIL34");
        _controls.BTN_AVAIL35 = findControl<gui::Button>("BTN_AVAIL35");
        _controls.BTN_CHOSEN0 = findControl<gui::Button>("BTN_CHOSEN0");
        _controls.BTN_CHOSEN1 = findControl<gui::Button>("BTN_CHOSEN1");
        _controls.BTN_CHOSEN2 = findControl<gui::Button>("BTN_CHOSEN2");
        _controls.BTN_CHOSEN3 = findControl<gui::Button>("BTN_CHOSEN3");
        _controls.BTN_CHOSEN4 = findControl<gui::Button>("BTN_CHOSEN4");
        _controls.BTN_CHOSEN5 = findControl<gui::Button>("BTN_CHOSEN5");
        _controls.BTN_CHOSEN6 = findControl<gui::Button>("BTN_CHOSEN6");
        _controls.BTN_CHOSEN7 = findControl<gui::Button>("BTN_CHOSEN7");
        _controls.BTN_CHOSEN8 = findControl<gui::Button>("BTN_CHOSEN8");
        _controls.BTN_CHOSEN9 = findControl<gui::Button>("BTN_CHOSEN9");
        _controls.BTN_CLEARCARDS = findControl<gui::Button>("BTN_CLEARCARDS");
        _controls.BTN_YTEXT = findControl<gui::Button>("BTN_YTEXT");
        _controls.LBL_AVAIL00 = findControl<gui::Label>("LBL_AVAIL00");
        _controls.LBL_AVAIL01 = findControl<gui::Label>("LBL_AVAIL01");
        _controls.LBL_AVAIL02 = findControl<gui::Label>("LBL_AVAIL02");
        _controls.LBL_AVAIL03 = findControl<gui::Label>("LBL_AVAIL03");
        _controls.LBL_AVAIL04 = findControl<gui::Label>("LBL_AVAIL04");
        _controls.LBL_AVAIL05 = findControl<gui::Label>("LBL_AVAIL05");
        _controls.LBL_AVAIL10 = findControl<gui::Label>("LBL_AVAIL10");
        _controls.LBL_AVAIL11 = findControl<gui::Label>("LBL_AVAIL11");
        _controls.LBL_AVAIL12 = findControl<gui::Label>("LBL_AVAIL12");
        _controls.LBL_AVAIL13 = findControl<gui::Label>("LBL_AVAIL13");
        _controls.LBL_AVAIL14 = findControl<gui::Label>("LBL_AVAIL14");
        _controls.LBL_AVAIL15 = findControl<gui::Label>("LBL_AVAIL15");
        _controls.LBL_AVAIL20 = findControl<gui::Label>("LBL_AVAIL20");
        _controls.LBL_AVAIL21 = findControl<gui::Label>("LBL_AVAIL21");
        _controls.LBL_AVAIL22 = findControl<gui::Label>("LBL_AVAIL22");
        _controls.LBL_AVAIL23 = findControl<gui::Label>("LBL_AVAIL23");
        _controls.LBL_AVAIL24 = findControl<gui::Label>("LBL_AVAIL24");
        _controls.LBL_AVAIL25 = findControl<gui::Label>("LBL_AVAIL25");
        _controls.LBL_AVAIL30 = findControl<gui::Label>("LBL_AVAIL30");
        _controls.LBL_AVAIL31 = findControl<gui::Label>("LBL_AVAIL31");
        _controls.LBL_AVAIL32 = findControl<gui::Label>("LBL_AVAIL32");
        _controls.LBL_AVAIL33 = findControl<gui::Label>("LBL_AVAIL33");
        _controls.LBL_AVAIL34 = findControl<gui::Label>("LBL_AVAIL34");
        _controls.LBL_AVAIL35 = findControl<gui::Label>("LBL_AVAIL35");
        _controls.LBL_AVAILNUM00 = findControl<gui::Label>("LBL_AVAILNUM00");
        _controls.LBL_AVAILNUM01 = findControl<gui::Label>("LBL_AVAILNUM01");
        _controls.LBL_AVAILNUM02 = findControl<gui::Label>("LBL_AVAILNUM02");
        _controls.LBL_AVAILNUM03 = findControl<gui::Label>("LBL_AVAILNUM03");
        _controls.LBL_AVAILNUM04 = findControl<gui::Label>("LBL_AVAILNUM04");
        _controls.LBL_AVAILNUM05 = findControl<gui::Label>("LBL_AVAILNUM05");
        _controls.LBL_AVAILNUM10 = findControl<gui::Label>("LBL_AVAILNUM10");
        _controls.LBL_AVAILNUM11 = findControl<gui::Label>("LBL_AVAILNUM11");
        _controls.LBL_AVAILNUM12 = findControl<gui::Label>("LBL_AVAILNUM12");
        _controls.LBL_AVAILNUM13 = findControl<gui::Label>("LBL_AVAILNUM13");
        _controls.LBL_AVAILNUM14 = findControl<gui::Label>("LBL_AVAILNUM14");
        _controls.LBL_AVAILNUM15 = findControl<gui::Label>("LBL_AVAILNUM15");
        _controls.LBL_AVAILNUM20 = findControl<gui::Label>("LBL_AVAILNUM20");
        _controls.LBL_AVAILNUM21 = findControl<gui::Label>("LBL_AVAILNUM21");
        _controls.LBL_AVAILNUM22 = findControl<gui::Label>("LBL_AVAILNUM22");
        _controls.LBL_AVAILNUM23 = findControl<gui::Label>("LBL_AVAILNUM23");
        _controls.LBL_AVAILNUM24 = findControl<gui::Label>("LBL_AVAILNUM24");
        _controls.LBL_AVAILNUM25 = findControl<gui::Label>("LBL_AVAILNUM25");
        _controls.LBL_AVAILNUM30 = findControl<gui::Label>("LBL_AVAILNUM30");
        _controls.LBL_AVAILNUM31 = findControl<gui::Label>("LBL_AVAILNUM31");
        _controls.LBL_AVAILNUM32 = findControl<gui::Label>("LBL_AVAILNUM32");
        _controls.LBL_AVAILNUM33 = findControl<gui::Label>("LBL_AVAILNUM33");
        _controls.LBL_AVAILNUM34 = findControl<gui::Label>("LBL_AVAILNUM34");
        _controls.LBL_AVAILNUM35 = findControl<gui::Label>("LBL_AVAILNUM35");
        _controls.LBL_CHOSEN0 = findControl<gui::Label>("LBL_CHOSEN0");
        _controls.LBL_CHOSEN1 = findControl<gui::Label>("LBL_CHOSEN1");
        _controls.LBL_CHOSEN2 = findControl<gui::Label>("LBL_CHOSEN2");
        _controls.LBL_CHOSEN3 = findControl<gui::Label>("LBL_CHOSEN3");
        _controls.LBL_CHOSEN4 = findControl<gui::Label>("LBL_CHOSEN4");
        _controls.LBL_CHOSEN5 = findControl<gui::Label>("LBL_CHOSEN5");
        _controls.LBL_CHOSEN6 = findControl<gui::Label>("LBL_CHOSEN6");
        _controls.LBL_CHOSEN7 = findControl<gui::Label>("LBL_CHOSEN7");
        _controls.LBL_CHOSEN8 = findControl<gui::Label>("LBL_CHOSEN8");
        _controls.LBL_CHOSEN9 = findControl<gui::Label>("LBL_CHOSEN9");
        _controls.LBL_HELP = findControl<gui::Label>("LBL_HELP");
        _controls.LBL_LTEXT = findControl<gui::Label>("LBL_LTEXT");
        _controls.LBL_RTEXT = findControl<gui::Label>("LBL_RTEXT");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ATEXT;
        std::shared_ptr<gui::Button> BTN_AVAIL00;
        std::shared_ptr<gui::Button> BTN_AVAIL01;
        std::shared_ptr<gui::Button> BTN_AVAIL02;
        std::shared_ptr<gui::Button> BTN_AVAIL03;
        std::shared_ptr<gui::Button> BTN_AVAIL04;
        std::shared_ptr<gui::Button> BTN_AVAIL05;
        std::shared_ptr<gui::Button> BTN_AVAIL10;
        std::shared_ptr<gui::Button> BTN_AVAIL11;
        std::shared_ptr<gui::Button> BTN_AVAIL12;
        std::shared_ptr<gui::Button> BTN_AVAIL13;
        std::shared_ptr<gui::Button> BTN_AVAIL14;
        std::shared_ptr<gui::Button> BTN_AVAIL15;
        std::shared_ptr<gui::Button> BTN_AVAIL20;
        std::shared_ptr<gui::Button> BTN_AVAIL21;
        std::shared_ptr<gui::Button> BTN_AVAIL22;
        std::shared_ptr<gui::Button> BTN_AVAIL23;
        std::shared_ptr<gui::Button> BTN_AVAIL24;
        std::shared_ptr<gui::Button> BTN_AVAIL25;
        std::shared_ptr<gui::Button> BTN_AVAIL30;
        std::shared_ptr<gui::Button> BTN_AVAIL31;
        std::shared_ptr<gui::Button> BTN_AVAIL32;
        std::shared_ptr<gui::Button> BTN_AVAIL33;
        std::shared_ptr<gui::Button> BTN_AVAIL34;
        std::shared_ptr<gui::Button> BTN_AVAIL35;
        std::shared_ptr<gui::Button> BTN_CHOSEN0;
        std::shared_ptr<gui::Button> BTN_CHOSEN1;
        std::shared_ptr<gui::Button> BTN_CHOSEN2;
        std::shared_ptr<gui::Button> BTN_CHOSEN3;
        std::shared_ptr<gui::Button> BTN_CHOSEN4;
        std::shared_ptr<gui::Button> BTN_CHOSEN5;
        std::shared_ptr<gui::Button> BTN_CHOSEN6;
        std::shared_ptr<gui::Button> BTN_CHOSEN7;
        std::shared_ptr<gui::Button> BTN_CHOSEN8;
        std::shared_ptr<gui::Button> BTN_CHOSEN9;
        std::shared_ptr<gui::Button> BTN_CLEARCARDS;
        std::shared_ptr<gui::Button> BTN_YTEXT;
        std::shared_ptr<gui::Label> LBL_AVAIL00;
        std::shared_ptr<gui::Label> LBL_AVAIL01;
        std::shared_ptr<gui::Label> LBL_AVAIL02;
        std::shared_ptr<gui::Label> LBL_AVAIL03;
        std::shared_ptr<gui::Label> LBL_AVAIL04;
        std::shared_ptr<gui::Label> LBL_AVAIL05;
        std::shared_ptr<gui::Label> LBL_AVAIL10;
        std::shared_ptr<gui::Label> LBL_AVAIL11;
        std::shared_ptr<gui::Label> LBL_AVAIL12;
        std::shared_ptr<gui::Label> LBL_AVAIL13;
        std::shared_ptr<gui::Label> LBL_AVAIL14;
        std::shared_ptr<gui::Label> LBL_AVAIL15;
        std::shared_ptr<gui::Label> LBL_AVAIL20;
        std::shared_ptr<gui::Label> LBL_AVAIL21;
        std::shared_ptr<gui::Label> LBL_AVAIL22;
        std::shared_ptr<gui::Label> LBL_AVAIL23;
        std::shared_ptr<gui::Label> LBL_AVAIL24;
        std::shared_ptr<gui::Label> LBL_AVAIL25;
        std::shared_ptr<gui::Label> LBL_AVAIL30;
        std::shared_ptr<gui::Label> LBL_AVAIL31;
        std::shared_ptr<gui::Label> LBL_AVAIL32;
        std::shared_ptr<gui::Label> LBL_AVAIL33;
        std::shared_ptr<gui::Label> LBL_AVAIL34;
        std::shared_ptr<gui::Label> LBL_AVAIL35;
        std::shared_ptr<gui::Label> LBL_AVAILNUM00;
        std::shared_ptr<gui::Label> LBL_AVAILNUM01;
        std::shared_ptr<gui::Label> LBL_AVAILNUM02;
        std::shared_ptr<gui::Label> LBL_AVAILNUM03;
        std::shared_ptr<gui::Label> LBL_AVAILNUM04;
        std::shared_ptr<gui::Label> LBL_AVAILNUM05;
        std::shared_ptr<gui::Label> LBL_AVAILNUM10;
        std::shared_ptr<gui::Label> LBL_AVAILNUM11;
        std::shared_ptr<gui::Label> LBL_AVAILNUM12;
        std::shared_ptr<gui::Label> LBL_AVAILNUM13;
        std::shared_ptr<gui::Label> LBL_AVAILNUM14;
        std::shared_ptr<gui::Label> LBL_AVAILNUM15;
        std::shared_ptr<gui::Label> LBL_AVAILNUM20;
        std::shared_ptr<gui::Label> LBL_AVAILNUM21;
        std::shared_ptr<gui::Label> LBL_AVAILNUM22;
        std::shared_ptr<gui::Label> LBL_AVAILNUM23;
        std::shared_ptr<gui::Label> LBL_AVAILNUM24;
        std::shared_ptr<gui::Label> LBL_AVAILNUM25;
        std::shared_ptr<gui::Label> LBL_AVAILNUM30;
        std::shared_ptr<gui::Label> LBL_AVAILNUM31;
        std::shared_ptr<gui::Label> LBL_AVAILNUM32;
        std::shared_ptr<gui::Label> LBL_AVAILNUM33;
        std::shared_ptr<gui::Label> LBL_AVAILNUM34;
        std::shared_ptr<gui::Label> LBL_AVAILNUM35;
        std::shared_ptr<gui::Label> LBL_CHOSEN0;
        std::shared_ptr<gui::Label> LBL_CHOSEN1;
        std::shared_ptr<gui::Label> LBL_CHOSEN2;
        std::shared_ptr<gui::Label> LBL_CHOSEN3;
        std::shared_ptr<gui::Label> LBL_CHOSEN4;
        std::shared_ptr<gui::Label> LBL_CHOSEN5;
        std::shared_ptr<gui::Label> LBL_CHOSEN6;
        std::shared_ptr<gui::Label> LBL_CHOSEN7;
        std::shared_ptr<gui::Label> LBL_CHOSEN8;
        std::shared_ptr<gui::Label> LBL_CHOSEN9;
        std::shared_ptr<gui::Label> LBL_HELP;
        std::shared_ptr<gui::Label> LBL_LTEXT;
        std::shared_ptr<gui::Label> LBL_RTEXT;
        std::shared_ptr<gui::Label> LBL_TITLE;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
