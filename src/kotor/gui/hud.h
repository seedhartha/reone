/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "../../gui/control/button.h"
#include "../../gui/control/label.h"
#include "../../gui/control/progressbar.h"
#include "../../gui/control/togglebutton.h"
#include "../../resource/types.h"

#include "barkbubble.h"
#include "gui.h"
#include "selectoverlay.h"

namespace reone {

namespace game {

class ActionFactory;
class Reputes;
class Skills;

} // namespace game

namespace kotor {

class HUD : public GameGUI {
public:
    HUD(KotOR &game, game::Services &services);

    void load() override;

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void draw() override;

    BarkBubble &barkBubble() const { return *_barkBubble; }

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnAbi;
        std::shared_ptr<gui::Button> btnAction0;
        std::shared_ptr<gui::Button> btnAction1;
        std::shared_ptr<gui::Button> btnAction2;
        std::shared_ptr<gui::Button> btnAction3;
        std::shared_ptr<gui::Button> btnActionDown0;
        std::shared_ptr<gui::Button> btnActionDown1;
        std::shared_ptr<gui::Button> btnActionDown2;
        std::shared_ptr<gui::Button> btnActionDown3;
        std::shared_ptr<gui::Button> btnActionUp0;
        std::shared_ptr<gui::Button> btnActionUp1;
        std::shared_ptr<gui::Button> btnActionUp2;
        std::shared_ptr<gui::Button> btnActionUp3;
        std::shared_ptr<gui::Button> btnChar;
        std::shared_ptr<gui::Button> btnChar1;
        std::shared_ptr<gui::Button> btnChar2;
        std::shared_ptr<gui::Button> btnChar3;
        std::shared_ptr<gui::Button> btnClearAll;
        std::shared_ptr<gui::Button> btnClearOne;
        std::shared_ptr<gui::Button> btnClearOne2;
        std::shared_ptr<gui::Button> btnEqu;
        std::shared_ptr<gui::Button> btnInv;
        std::shared_ptr<gui::Button> btnJou;
        std::shared_ptr<gui::Button> btnMap;
        std::shared_ptr<gui::Button> btnMinimap;
        std::shared_ptr<gui::Button> btnMsg;
        std::shared_ptr<gui::Button> btnOpt;
        std::shared_ptr<gui::Button> btnTarget0;
        std::shared_ptr<gui::Button> btnTarget1;
        std::shared_ptr<gui::Button> btnTarget2;
        std::shared_ptr<gui::Button> btnTargetDown0;
        std::shared_ptr<gui::Button> btnTargetDown1;
        std::shared_ptr<gui::Button> btnTargetDown2;
        std::shared_ptr<gui::Button> btnTargetUp0;
        std::shared_ptr<gui::Button> btnTargetUp1;
        std::shared_ptr<gui::Button> btnTargetUp2;
        std::shared_ptr<gui::Label> lblAction0;
        std::shared_ptr<gui::Label> lblAction1;
        std::shared_ptr<gui::Label> lblAction2;
        std::shared_ptr<gui::Label> lblAction3;
        std::shared_ptr<gui::Label> lblActionDesc;
        std::shared_ptr<gui::Label> lblActionDescBg;
        std::shared_ptr<gui::Label> lblArrow;
        std::shared_ptr<gui::Label> lblArrowMargin;
        std::shared_ptr<gui::Label> lblBack1;
        std::shared_ptr<gui::Label> lblBack2;
        std::shared_ptr<gui::Label> lblBack3;
        std::shared_ptr<gui::Label> lblCash;
        std::shared_ptr<gui::Label> lblChar1;
        std::shared_ptr<gui::Label> lblChar2;
        std::shared_ptr<gui::Label> lblChar3;
        std::shared_ptr<gui::Label> lblCmbtEfctInc1;
        std::shared_ptr<gui::Label> lblCmbtEfctInc2;
        std::shared_ptr<gui::Label> lblCmbtEfctInc3;
        std::shared_ptr<gui::Label> lblCmbtEfctRed1;
        std::shared_ptr<gui::Label> lblCmbtEfctRed2;
        std::shared_ptr<gui::Label> lblCmbtEfctRed3;
        std::shared_ptr<gui::Label> lblCmbtModeMsg;
        std::shared_ptr<gui::Label> lblCmbtMsgBg;
        std::shared_ptr<gui::Label> lblCombatBg3;
        std::shared_ptr<gui::Label> lblDarkShift;
        std::shared_ptr<gui::Label> lblDebilatated1;
        std::shared_ptr<gui::Label> lblDebilatated2;
        std::shared_ptr<gui::Label> lblDebilatated3;
        std::shared_ptr<gui::Label> lblDisable1;
        std::shared_ptr<gui::Label> lblDisable2;
        std::shared_ptr<gui::Label> lblDisable3;
        std::shared_ptr<gui::Label> lblHealthBg;
        std::shared_ptr<gui::Label> lblItemLost;
        std::shared_ptr<gui::Label> lblItemRcvd;
        std::shared_ptr<gui::Label> lblJournal;
        std::shared_ptr<gui::Label> lblLevelUp1;
        std::shared_ptr<gui::Label> lblLevelUp2;
        std::shared_ptr<gui::Label> lblLevelUp3;
        std::shared_ptr<gui::Label> lblLightShift;
        std::shared_ptr<gui::Label> lblMap;
        std::shared_ptr<gui::Label> lblMapBorder;
        std::shared_ptr<gui::Label> lblMapView;
        std::shared_ptr<gui::Label> lblMenuBg;
        std::shared_ptr<gui::Label> lblMoulding1;
        std::shared_ptr<gui::Label> lblMoulding3;
        std::shared_ptr<gui::Label> lblName;
        std::shared_ptr<gui::Label> lblNameBg;
        std::shared_ptr<gui::Label> lblPlotXp;
        std::shared_ptr<gui::Label> lblQueue0;
        std::shared_ptr<gui::Label> lblQueue1;
        std::shared_ptr<gui::Label> lblQueue2;
        std::shared_ptr<gui::Label> lblQueue3;
        std::shared_ptr<gui::Label> lblStealthXp;
        std::shared_ptr<gui::Label> lblTarget0;
        std::shared_ptr<gui::Label> lblTarget1;
        std::shared_ptr<gui::Label> lblTarget2;
        std::shared_ptr<gui::ProgressBar> pbForce1;
        std::shared_ptr<gui::ProgressBar> pbForce2;
        std::shared_ptr<gui::ProgressBar> pbForce3;
        std::shared_ptr<gui::ProgressBar> pbHealth;
        std::shared_ptr<gui::ProgressBar> pbVit1;
        std::shared_ptr<gui::ProgressBar> pbVit2;
        std::shared_ptr<gui::ProgressBar> pbVit3;
        std::shared_ptr<gui::ToggleButton> tbPause;
        std::shared_ptr<gui::ToggleButton> tbSolo;
        std::shared_ptr<gui::ToggleButton> tbStealth;

        // KotOR only
        std::shared_ptr<gui::Label> lblCombatBg1;
        std::shared_ptr<gui::Label> lblCombatBg2;
        std::shared_ptr<gui::Label> lblLvlUpBg1;
        std::shared_ptr<gui::Label> lblLvlUpBg2;
        std::shared_ptr<gui::Label> lblLvlUpBg3;
        std::shared_ptr<gui::Label> lblMoulding2;
        // END KotOR only

        // TSL only
        std::shared_ptr<gui::Button> btnAction4;
        std::shared_ptr<gui::Button> btnAction5;
        std::shared_ptr<gui::Button> btnActionDown4;
        std::shared_ptr<gui::Button> btnActionDown5;
        std::shared_ptr<gui::Button> btnActionUp4;
        std::shared_ptr<gui::Button> btnActionUp5;
        std::shared_ptr<gui::Button> btnSwapWeapons;
        std::shared_ptr<gui::Label> lblAction4;
        std::shared_ptr<gui::Label> lblAction5;
        // END TSL only
    } _binding;

    SelectionOverlay _select;
    std::unique_ptr<BarkBubble> _barkBubble;

    void bindControls();
    void toggleCombat(bool enabled);
    void refreshActionQueueItems() const;

    void drawHealth(int memberIndex);
    void drawMinimap();
};

} // namespace kotor

} // namespace reone
