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

#include "reone/game/gui/chargen/quick.h"

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"

#include "reone/game/game.h"

#include "reone/game/gui/chargen.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void QuickCharacterGeneration::onGUILoaded() {
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
    _binding.btnBack = findControl<Button>("BTN_BACK");
    _binding.btnCancel = findControl<Button>("BTN_CANCEL");
    _binding.btnStepName1 = findControl<Button>("BTN_STEPNAME1");
    _binding.btnStepName2 = findControl<Button>("BTN_STEPNAME2");
    _binding.btnStepName3 = findControl<Button>("BTN_STEPNAME3");
    _binding.lbl1 = findControl<Label>("LBL_1");
    _binding.lbl2 = findControl<Label>("LBL_2");
    _binding.lbl3 = findControl<Label>("LBL_3");
    _binding.lblDecoration = findControl<Label>("LBL_DECORATION");
    _binding.lblNum1 = findControl<Label>("LBL_NUM1");
    _binding.lblNum2 = findControl<Label>("LBL_NUM2");
    _binding.lblNum3 = findControl<Label>("LBL_NUM3");
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
