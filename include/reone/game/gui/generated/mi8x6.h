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

#include "reone/gui/control/imagebutton.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/progressbar.h"
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_mi8x6 : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.LBH_ARROW1 = findControl<gui::ImageButton>("LBH_ARROW1");
        _controls.LBH_ARROW1B = findControl<gui::ImageButton>("LBH_ARROW1B");
        _controls.LBH_ARROW2 = findControl<gui::ImageButton>("LBH_ARROW2");
        _controls.LBH_ARROW2B = findControl<gui::ImageButton>("LBH_ARROW2B");
        _controls.LBH_ARROW3 = findControl<gui::ImageButton>("LBH_ARROW3");
        _controls.LBH_ARROW3B = findControl<gui::ImageButton>("LBH_ARROW3B");
        _controls.LBH_ARROW4 = findControl<gui::ImageButton>("LBH_ARROW4");
        _controls.LBH_ARROW4B = findControl<gui::ImageButton>("LBH_ARROW4B");
        _controls.LBH_ARROW5 = findControl<gui::ImageButton>("LBH_ARROW5");
        _controls.LBH_ARROW5B = findControl<gui::ImageButton>("LBH_ARROW5B");
        _controls.LBH_ARROW6 = findControl<gui::ImageButton>("LBH_ARROW6");
        _controls.LBH_ARROW6B = findControl<gui::ImageButton>("LBH_ARROW6B");
        _controls.LBH_BORDER1 = findControl<gui::ImageButton>("LBH_BORDER1");
        _controls.LBH_BORDER1B = findControl<gui::ImageButton>("LBH_BORDER1B");
        _controls.LBH_BORDER2 = findControl<gui::ImageButton>("LBH_BORDER2");
        _controls.LBH_BORDER2B = findControl<gui::ImageButton>("LBH_BORDER2B");
        _controls.LBH_BORDER3 = findControl<gui::ImageButton>("LBH_BORDER3");
        _controls.LBH_BORDER3B = findControl<gui::ImageButton>("LBH_BORDER3B");
        _controls.LBH_BORDER4 = findControl<gui::ImageButton>("LBH_BORDER4");
        _controls.LBH_BORDER4B = findControl<gui::ImageButton>("LBH_BORDER4B");
        _controls.LBH_BORDER5 = findControl<gui::ImageButton>("LBH_BORDER5");
        _controls.LBH_BORDER5B = findControl<gui::ImageButton>("LBH_BORDER5B");
        _controls.LBH_BORDER6 = findControl<gui::ImageButton>("LBH_BORDER6");
        _controls.LBH_BORDER6B = findControl<gui::ImageButton>("LBH_BORDER6B");
        _controls.LBL_ACTIONDESC = findControl<gui::Label>("LBL_ACTIONDESC");
        _controls.LBL_ACTIONDESCBG = findControl<gui::Label>("LBL_ACTIONDESCBG");
        _controls.LBL_ACTIONDESCBG2 = findControl<gui::Label>("LBL_ACTIONDESCBG2");
        _controls.LBL_ARROW = findControl<gui::Label>("LBL_ARROW");
        _controls.LBL_BACK1 = findControl<gui::Label>("LBL_BACK1");
        _controls.LBL_BACK2 = findControl<gui::Label>("LBL_BACK2");
        _controls.LBL_BACK3 = findControl<gui::Label>("LBL_BACK3");
        _controls.LBL_BACK4 = findControl<gui::Label>("LBL_BACK4");
        _controls.LBL_BG1 = findControl<gui::Label>("LBL_BG1");
        _controls.LBL_BG2 = findControl<gui::Label>("LBL_BG2");
        _controls.LBL_CASH = findControl<gui::Label>("LBL_CASH");
        _controls.LBL_CHAR1 = findControl<gui::Label>("LBL_CHAR1");
        _controls.LBL_CHAR2 = findControl<gui::Label>("LBL_CHAR2");
        _controls.LBL_CHAR3 = findControl<gui::Label>("LBL_CHAR3");
        _controls.LBL_CHAR4 = findControl<gui::Label>("LBL_CHAR4");
        _controls.LBL_CMBTEFCTINC1 = findControl<gui::Label>("LBL_CMBTEFCTINC1");
        _controls.LBL_CMBTEFCTINC2 = findControl<gui::Label>("LBL_CMBTEFCTINC2");
        _controls.LBL_CMBTEFCTINC3 = findControl<gui::Label>("LBL_CMBTEFCTINC3");
        _controls.LBL_CMBTEFCTINC4 = findControl<gui::Label>("LBL_CMBTEFCTINC4");
        _controls.LBL_CMBTEFCTRED1 = findControl<gui::Label>("LBL_CMBTEFCTRED1");
        _controls.LBL_CMBTEFCTRED2 = findControl<gui::Label>("LBL_CMBTEFCTRED2");
        _controls.LBL_CMBTEFCTRED3 = findControl<gui::Label>("LBL_CMBTEFCTRED3");
        _controls.LBL_CMBTEFCTRED4 = findControl<gui::Label>("LBL_CMBTEFCTRED4");
        _controls.LBL_DARKSHIFT = findControl<gui::Label>("LBL_DARKSHIFT");
        _controls.LBL_DEBILATATED1 = findControl<gui::Label>("LBL_DEBILATATED1");
        _controls.LBL_DEBILATATED2 = findControl<gui::Label>("LBL_DEBILATATED2");
        _controls.LBL_DEBILATATED3 = findControl<gui::Label>("LBL_DEBILATATED3");
        _controls.LBL_DEBILATATED4 = findControl<gui::Label>("LBL_DEBILATATED4");
        _controls.LBL_DISABLE1 = findControl<gui::Label>("LBL_DISABLE1");
        _controls.LBL_DISABLE2 = findControl<gui::Label>("LBL_DISABLE2");
        _controls.LBL_DISABLE3 = findControl<gui::Label>("LBL_DISABLE3");
        _controls.LBL_DISABLE4 = findControl<gui::Label>("LBL_DISABLE4");
        _controls.LBL_HIGHLIGHT = findControl<gui::ImageButton>("LBL_HIGHLIGHT");
        _controls.LBL_ICON1 = findControl<gui::Label>("LBL_ICON1");
        _controls.LBL_ICON1B = findControl<gui::Label>("LBL_ICON1B");
        _controls.LBL_ICON2 = findControl<gui::Label>("LBL_ICON2");
        _controls.LBL_ICON2B = findControl<gui::Label>("LBL_ICON2B");
        _controls.LBL_ICON3 = findControl<gui::Label>("LBL_ICON3");
        _controls.LBL_ICON3B = findControl<gui::Label>("LBL_ICON3B");
        _controls.LBL_ICON4 = findControl<gui::Label>("LBL_ICON4");
        _controls.LBL_ICON4B = findControl<gui::Label>("LBL_ICON4B");
        _controls.LBL_ICON5 = findControl<gui::Label>("LBL_ICON5");
        _controls.LBL_ICON5B = findControl<gui::Label>("LBL_ICON5B");
        _controls.LBL_ICON6 = findControl<gui::Label>("LBL_ICON6");
        _controls.LBL_ICON6B = findControl<gui::Label>("LBL_ICON6B");
        _controls.LBL_INDICATE = findControl<gui::Label>("LBL_INDICATE");
        _controls.LBL_ITEMLOST = findControl<gui::Label>("LBL_ITEMLOST");
        _controls.LBL_ITEMRCVD = findControl<gui::Label>("LBL_ITEMRCVD");
        _controls.LBL_JOURNAL = findControl<gui::Label>("LBL_JOURNAL");
        _controls.LBL_LEVELUP1 = findControl<gui::Label>("LBL_LEVELUP1");
        _controls.LBL_LEVELUP2 = findControl<gui::Label>("LBL_LEVELUP2");
        _controls.LBL_LEVELUP3 = findControl<gui::Label>("LBL_LEVELUP3");
        _controls.LBL_LEVELUP4 = findControl<gui::Label>("LBL_LEVELUP4");
        _controls.LBL_LIGHTSHIFT = findControl<gui::Label>("LBL_LIGHTSHIFT");
        _controls.LBL_LVLUPBG1 = findControl<gui::Label>("LBL_LVLUPBG1");
        _controls.LBL_LVLUPBG2 = findControl<gui::Label>("LBL_LVLUPBG2");
        _controls.LBL_LVLUPBG3 = findControl<gui::Label>("LBL_LVLUPBG3");
        _controls.LBL_LVLUPBG4 = findControl<gui::Label>("LBL_LVLUPBG4");
        _controls.LBL_MAP = findControl<gui::Label>("LBL_MAP");
        _controls.LBL_MAPBORDER = findControl<gui::Label>("LBL_MAPBORDER");
        _controls.LBL_MAPVIEW = findControl<gui::Label>("LBL_MAPVIEW");
        _controls.LBL_NAME = findControl<gui::Label>("LBL_NAME");
        _controls.LBL_PLOTXP = findControl<gui::Label>("LBL_PLOTXP");
        _controls.LBL_QUEUE0 = findControl<gui::Label>("LBL_QUEUE0");
        _controls.LBL_QUEUE1 = findControl<gui::Label>("LBL_QUEUE1");
        _controls.LBL_QUEUE2 = findControl<gui::Label>("LBL_QUEUE2");
        _controls.LBL_QUEUE3 = findControl<gui::Label>("LBL_QUEUE3");
        _controls.LBL_SOLOMODE = findControl<gui::Label>("LBL_SOLOMODE");
        _controls.LBL_STEALTHXP = findControl<gui::Label>("LBL_STEALTHXP");
        _controls.PB_FORCE1 = findControl<gui::ProgressBar>("PB_FORCE1");
        _controls.PB_FORCE2 = findControl<gui::ProgressBar>("PB_FORCE2");
        _controls.PB_FORCE3 = findControl<gui::ProgressBar>("PB_FORCE3");
        _controls.PB_FORCE4 = findControl<gui::ProgressBar>("PB_FORCE4");
        _controls.PB_HEALTH = findControl<gui::ProgressBar>("PB_HEALTH");
        _controls.PB_VIT1 = findControl<gui::ProgressBar>("PB_VIT1");
        _controls.PB_VIT2 = findControl<gui::ProgressBar>("PB_VIT2");
        _controls.PB_VIT3 = findControl<gui::ProgressBar>("PB_VIT3");
        _controls.PB_VIT4 = findControl<gui::ProgressBar>("PB_VIT4");
    }

private:
    struct Controls {
        std::shared_ptr<gui::ImageButton> LBH_ARROW1;
        std::shared_ptr<gui::ImageButton> LBH_ARROW1B;
        std::shared_ptr<gui::ImageButton> LBH_ARROW2;
        std::shared_ptr<gui::ImageButton> LBH_ARROW2B;
        std::shared_ptr<gui::ImageButton> LBH_ARROW3;
        std::shared_ptr<gui::ImageButton> LBH_ARROW3B;
        std::shared_ptr<gui::ImageButton> LBH_ARROW4;
        std::shared_ptr<gui::ImageButton> LBH_ARROW4B;
        std::shared_ptr<gui::ImageButton> LBH_ARROW5;
        std::shared_ptr<gui::ImageButton> LBH_ARROW5B;
        std::shared_ptr<gui::ImageButton> LBH_ARROW6;
        std::shared_ptr<gui::ImageButton> LBH_ARROW6B;
        std::shared_ptr<gui::ImageButton> LBH_BORDER1;
        std::shared_ptr<gui::ImageButton> LBH_BORDER1B;
        std::shared_ptr<gui::ImageButton> LBH_BORDER2;
        std::shared_ptr<gui::ImageButton> LBH_BORDER2B;
        std::shared_ptr<gui::ImageButton> LBH_BORDER3;
        std::shared_ptr<gui::ImageButton> LBH_BORDER3B;
        std::shared_ptr<gui::ImageButton> LBH_BORDER4;
        std::shared_ptr<gui::ImageButton> LBH_BORDER4B;
        std::shared_ptr<gui::ImageButton> LBH_BORDER5;
        std::shared_ptr<gui::ImageButton> LBH_BORDER5B;
        std::shared_ptr<gui::ImageButton> LBH_BORDER6;
        std::shared_ptr<gui::ImageButton> LBH_BORDER6B;
        std::shared_ptr<gui::Label> LBL_ACTIONDESC;
        std::shared_ptr<gui::Label> LBL_ACTIONDESCBG;
        std::shared_ptr<gui::Label> LBL_ACTIONDESCBG2;
        std::shared_ptr<gui::Label> LBL_ARROW;
        std::shared_ptr<gui::Label> LBL_BACK1;
        std::shared_ptr<gui::Label> LBL_BACK2;
        std::shared_ptr<gui::Label> LBL_BACK3;
        std::shared_ptr<gui::Label> LBL_BACK4;
        std::shared_ptr<gui::Label> LBL_BG1;
        std::shared_ptr<gui::Label> LBL_BG2;
        std::shared_ptr<gui::Label> LBL_CASH;
        std::shared_ptr<gui::Label> LBL_CHAR1;
        std::shared_ptr<gui::Label> LBL_CHAR2;
        std::shared_ptr<gui::Label> LBL_CHAR3;
        std::shared_ptr<gui::Label> LBL_CHAR4;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTINC1;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTINC2;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTINC3;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTINC4;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTRED1;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTRED2;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTRED3;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTRED4;
        std::shared_ptr<gui::Label> LBL_DARKSHIFT;
        std::shared_ptr<gui::Label> LBL_DEBILATATED1;
        std::shared_ptr<gui::Label> LBL_DEBILATATED2;
        std::shared_ptr<gui::Label> LBL_DEBILATATED3;
        std::shared_ptr<gui::Label> LBL_DEBILATATED4;
        std::shared_ptr<gui::Label> LBL_DISABLE1;
        std::shared_ptr<gui::Label> LBL_DISABLE2;
        std::shared_ptr<gui::Label> LBL_DISABLE3;
        std::shared_ptr<gui::Label> LBL_DISABLE4;
        std::shared_ptr<gui::ImageButton> LBL_HIGHLIGHT;
        std::shared_ptr<gui::Label> LBL_ICON1;
        std::shared_ptr<gui::Label> LBL_ICON1B;
        std::shared_ptr<gui::Label> LBL_ICON2;
        std::shared_ptr<gui::Label> LBL_ICON2B;
        std::shared_ptr<gui::Label> LBL_ICON3;
        std::shared_ptr<gui::Label> LBL_ICON3B;
        std::shared_ptr<gui::Label> LBL_ICON4;
        std::shared_ptr<gui::Label> LBL_ICON4B;
        std::shared_ptr<gui::Label> LBL_ICON5;
        std::shared_ptr<gui::Label> LBL_ICON5B;
        std::shared_ptr<gui::Label> LBL_ICON6;
        std::shared_ptr<gui::Label> LBL_ICON6B;
        std::shared_ptr<gui::Label> LBL_INDICATE;
        std::shared_ptr<gui::Label> LBL_ITEMLOST;
        std::shared_ptr<gui::Label> LBL_ITEMRCVD;
        std::shared_ptr<gui::Label> LBL_JOURNAL;
        std::shared_ptr<gui::Label> LBL_LEVELUP1;
        std::shared_ptr<gui::Label> LBL_LEVELUP2;
        std::shared_ptr<gui::Label> LBL_LEVELUP3;
        std::shared_ptr<gui::Label> LBL_LEVELUP4;
        std::shared_ptr<gui::Label> LBL_LIGHTSHIFT;
        std::shared_ptr<gui::Label> LBL_LVLUPBG1;
        std::shared_ptr<gui::Label> LBL_LVLUPBG2;
        std::shared_ptr<gui::Label> LBL_LVLUPBG3;
        std::shared_ptr<gui::Label> LBL_LVLUPBG4;
        std::shared_ptr<gui::Label> LBL_MAP;
        std::shared_ptr<gui::Label> LBL_MAPBORDER;
        std::shared_ptr<gui::Label> LBL_MAPVIEW;
        std::shared_ptr<gui::Label> LBL_NAME;
        std::shared_ptr<gui::Label> LBL_PLOTXP;
        std::shared_ptr<gui::Label> LBL_QUEUE0;
        std::shared_ptr<gui::Label> LBL_QUEUE1;
        std::shared_ptr<gui::Label> LBL_QUEUE2;
        std::shared_ptr<gui::Label> LBL_QUEUE3;
        std::shared_ptr<gui::Label> LBL_SOLOMODE;
        std::shared_ptr<gui::Label> LBL_STEALTHXP;
        std::shared_ptr<gui::ProgressBar> PB_FORCE1;
        std::shared_ptr<gui::ProgressBar> PB_FORCE2;
        std::shared_ptr<gui::ProgressBar> PB_FORCE3;
        std::shared_ptr<gui::ProgressBar> PB_FORCE4;
        std::shared_ptr<gui::ProgressBar> PB_HEALTH;
        std::shared_ptr<gui::ProgressBar> PB_VIT1;
        std::shared_ptr<gui::ProgressBar> PB_VIT2;
        std::shared_ptr<gui::ProgressBar> PB_VIT3;
        std::shared_ptr<gui::ProgressBar> PB_VIT4;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
