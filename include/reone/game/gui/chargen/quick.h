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

#include "../../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;

} // namespace gui

namespace game {

class CharacterGeneration;

class QuickCharacterGeneration : public GameGUI {
public:
    QuickCharacterGeneration(
        CharacterGeneration &charGen,
        Game &game,
        ServicesView &services) :
        GameGUI(game, services),
        _charGen(charGen) {
        _resRef = guiResRef("quickpnl");
    }

    void goToNextStep();

    int step() const { return _step; }

    void setStep(int step);

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_CANCEL;
        std::shared_ptr<gui::Button> BTN_STEPNAME1;
        std::shared_ptr<gui::Button> BTN_STEPNAME2;
        std::shared_ptr<gui::Button> BTN_STEPNAME3;
        std::shared_ptr<gui::ImageButton> LBL_1;
        std::shared_ptr<gui::ImageButton> LBL_2;
        std::shared_ptr<gui::ImageButton> LBL_3;
        std::shared_ptr<gui::Label> LBL_DECORATION;
        std::shared_ptr<gui::Label> LBL_NUM1;
        std::shared_ptr<gui::Label> LBL_NUM2;
        std::shared_ptr<gui::Label> LBL_NUM3;
    };

    Controls _controls;

    CharacterGeneration &_charGen;
    int _step {0};

    void onGUILoaded() override;

    void bindControls() {
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_CANCEL = findControl<gui::Button>("BTN_CANCEL");
        _controls.BTN_STEPNAME1 = findControl<gui::Button>("BTN_STEPNAME1");
        _controls.BTN_STEPNAME2 = findControl<gui::Button>("BTN_STEPNAME2");
        _controls.BTN_STEPNAME3 = findControl<gui::Button>("BTN_STEPNAME3");
        _controls.LBL_1 = findControl<gui::ImageButton>("LBL_1");
        _controls.LBL_2 = findControl<gui::ImageButton>("LBL_2");
        _controls.LBL_3 = findControl<gui::ImageButton>("LBL_3");
        _controls.LBL_DECORATION = findControl<gui::Label>("LBL_DECORATION");
        _controls.LBL_NUM1 = findControl<gui::Label>("LBL_NUM1");
        _controls.LBL_NUM2 = findControl<gui::Label>("LBL_NUM2");
        _controls.LBL_NUM3 = findControl<gui::Label>("LBL_NUM3");
    }

    void doSetStep(int step);
};

} // namespace game

} // namespace reone
