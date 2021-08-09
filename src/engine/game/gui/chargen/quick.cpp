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

#include "quick.h"

#include "../../game.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

QuickCharacterGeneration::QuickCharacterGeneration(CharacterGeneration *charGen, Game *game) :
    GameGUI(game),
    _charGen(charGen) {

    _resRef = getResRef("quickpnl");

    initForGame();
}

void QuickCharacterGeneration::load() {
    GUI::load();
    bindControls();
    doSetStep(0);

    if (_game->id() == GameID::KotOR) {
        _binding.lblDecoration->setDiscardColor(glm::vec3(0.0f, 0.0f, 0.082353f));
    }
}

void QuickCharacterGeneration::bindControls() {
    _binding.btnStepName1 = getControlPtr<Button>("BTN_STEPNAME1");
    _binding.btnStepName2 = getControlPtr<Button>("BTN_STEPNAME2");
    _binding.btnStepName3 = getControlPtr<Button>("BTN_STEPNAME3");
    _binding.lbl1 = getControlPtr<Label>("LBL_1");
    _binding.lbl2 = getControlPtr<Label>("LBL_2");
    _binding.lbl3 = getControlPtr<Label>("LBL_3");
    _binding.lblDecoration = getControlPtr<Label>("LBL_DECORATION");
    _binding.lblNum1 = getControlPtr<Label>("LBL_NUM1");
    _binding.lblNum2 = getControlPtr<Label>("LBL_NUM2");
    _binding.lblNum3 = getControlPtr<Label>("LBL_NUM3");
}

void QuickCharacterGeneration::setStep(int step) {
    if (_step != step) {
        doSetStep(step);
    }
}

void QuickCharacterGeneration::doSetStep(int step) {
    _step = step;

    _binding.lbl1->setFocusable(false);
    _binding.lbl2->setFocusable(false);
    _binding.lbl3->setFocusable(false);
    _binding.btnStepName1->setFocusable(false);
    _binding.btnStepName2->setFocusable(false);
    _binding.btnStepName3->setFocusable(false);

    _binding.lbl1->setDisabled(_step != 0);
    _binding.lbl2->setDisabled(_step != 1);
    _binding.lbl3->setDisabled(_step != 2);
    _binding.btnStepName1->setDisabled(_step != 0);
    _binding.btnStepName2->setDisabled(_step != 1);
    _binding.btnStepName3->setDisabled(_step != 2);

    _binding.lbl1->setFocus(_step == 0);
    _binding.lbl2->setFocus(_step == 1);
    _binding.lbl3->setFocus(_step == 2);
    _binding.lblNum1->setFocus(_step == 0);
    _binding.lblNum2->setFocus(_step == 1);
    _binding.lblNum3->setFocus(_step == 2);
    _binding.btnStepName1->setFocus(_step == 0);
    _binding.btnStepName2->setFocus(_step == 1);
    _binding.btnStepName3->setFocus(_step == 2);
}

void QuickCharacterGeneration::goToNextStep() {
    if (_step < 3) {
        doSetStep(_step + 1);
    }
}

void QuickCharacterGeneration::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_CANCEL") {
        setStep(0);
        _charGen->openQuickOrCustom();

    } else if (control == "BTN_BACK") {
        if (_step == 0) {
            _charGen->openQuickOrCustom();
        } else {
            setStep(_step - 1);
        }
    } else if (boost::starts_with(control, "BTN_STEPNAME")) {
        int step = control[12] - '0';
        switch (step) {
            case 1:
                _charGen->openPortraitSelection();
                break;
            case 2:
                _charGen->openNameEntry();
                break;
            default:
                _charGen->finish();
                break;
        }
    }
}

} // namespace game

} // namespace reone
