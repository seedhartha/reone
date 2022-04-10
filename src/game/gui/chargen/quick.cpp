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

#include "../../../gui/control/button.h"
#include "../../../gui/control/label.h"

#include "../../game.h"

#include "../chargen.h"

using namespace std;

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

QuickCharacterGeneration::QuickCharacterGeneration(
    CharacterGeneration &charGen,
    Game &game,
    GameServices &services) :
    GameGUI(game, services),
    _charGen(charGen) {

    _resRef = getResRef("quickpnl");

    initForGame();
}

void QuickCharacterGeneration::load() {
    GUI::load();
    bindControls();
    doSetStep(0);

    if (!_game.isTSL()) {
        _binding.lblDecoration->setDiscardColor(glm::vec3(0.0f, 0.0f, 0.082353f));
    }

    _binding.btnCancel->setOnClick([this]() {
        setStep(0);
        _charGen.openQuickOrCustom();
    });
    _binding.btnBack->setOnClick([this]() {
        if (_step == 0) {
            _charGen.openQuickOrCustom();
        } else {
            setStep(_step - 1);
        }
    });
    _binding.btnStepName1->setOnClick([this]() {
        _charGen.openPortraitSelection();
    });
    _binding.btnStepName2->setOnClick([this]() {
        _charGen.openNameEntry();
    });
    _binding.btnStepName3->setOnClick([this]() {
        _charGen.finish();
    });
}

void QuickCharacterGeneration::bindControls() {
    _binding.btnBack = getControl<Button>("BTN_BACK");
    _binding.btnCancel = getControl<Button>("BTN_CANCEL");
    _binding.btnStepName1 = getControl<Button>("BTN_STEPNAME1");
    _binding.btnStepName2 = getControl<Button>("BTN_STEPNAME2");
    _binding.btnStepName3 = getControl<Button>("BTN_STEPNAME3");
    _binding.lbl1 = getControl<Label>("LBL_1");
    _binding.lbl2 = getControl<Label>("LBL_2");
    _binding.lbl3 = getControl<Label>("LBL_3");
    _binding.lblDecoration = getControl<Label>("LBL_DECORATION");
    _binding.lblNum1 = getControl<Label>("LBL_NUM1");
    _binding.lblNum2 = getControl<Label>("LBL_NUM2");
    _binding.lblNum3 = getControl<Label>("LBL_NUM3");
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

} // namespace game

} // namespace reone
