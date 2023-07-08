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

#include "reone/game/gui/chargen/custom.h"

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"

#include "reone/game/game.h"

#include "reone/game/gui/chargen.h"

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void CustomCharacterGeneration::onGUILoaded() {
    bindControls();

    doSetStep(0);

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
        _charGen.openAbilities();
    });
    _binding.btnStepName3->setOnClick([this]() {
        _charGen.openSkills();
    });
    _binding.btnStepName4->setOnClick([this]() {
        _charGen.openFeats();
    });
    _binding.btnStepName5->setOnClick([this]() {
        _charGen.openNameEntry();
    });
    _binding.btnStepName6->setOnClick([this]() {
        _charGen.finish();
    });

    if (!_game.isTSL()) {
        _binding.lblBg->setDiscardColor(glm::vec3(0.0f, 0.0f, 0.082353f));
    }
}

void CustomCharacterGeneration::bindControls() {
    _binding.btnCancel = findControl<Button>("BTN_CANCEL");
    _binding.btnBack = findControl<Button>("BTN_BACK");
    _binding.btnStepName1 = findControl<Button>("BTN_STEPNAME1");
    _binding.btnStepName2 = findControl<Button>("BTN_STEPNAME2");
    _binding.btnStepName3 = findControl<Button>("BTN_STEPNAME3");
    _binding.btnStepName4 = findControl<Button>("BTN_STEPNAME4");
    _binding.btnStepName5 = findControl<Button>("BTN_STEPNAME5");
    _binding.btnStepName6 = findControl<Button>("BTN_STEPNAME6");
    _binding.lbl1 = findControl<Label>("LBL_1");
    _binding.lbl2 = findControl<Label>("LBL_2");
    _binding.lbl3 = findControl<Label>("LBL_3");
    _binding.lbl4 = findControl<Label>("LBL_4");
    _binding.lbl5 = findControl<Label>("LBL_5");
    _binding.lbl6 = findControl<Label>("LBL_6");
    _binding.lblBg = findControl<Label>("LBL_BG");
    _binding.lblNum1 = findControl<Label>("LBL_NUM1");
    _binding.lblNum2 = findControl<Label>("LBL_NUM2");
    _binding.lblNum3 = findControl<Label>("LBL_NUM3");
    _binding.lblNum4 = findControl<Label>("LBL_NUM4");
    _binding.lblNum5 = findControl<Label>("LBL_NUM5");
    _binding.lblNum6 = findControl<Label>("LBL_NUM6");
}

void CustomCharacterGeneration::setStep(int step) {
    if (_step != step) {
        doSetStep(step);
    }
}

void CustomCharacterGeneration::doSetStep(int step) {
    _step = step;

    _binding.lbl1->setFocusable(false);
    _binding.lbl2->setFocusable(false);
    _binding.lbl3->setFocusable(false);
    _binding.lbl4->setFocusable(false);
    _binding.lbl5->setFocusable(false);
    _binding.lbl6->setFocusable(false);
    _binding.btnStepName1->setFocusable(false);
    _binding.btnStepName2->setFocusable(false);
    _binding.btnStepName3->setFocusable(false);
    _binding.btnStepName4->setFocusable(false);
    _binding.btnStepName5->setFocusable(false);
    _binding.btnStepName6->setFocusable(false);

    _binding.lbl1->setDisabled(_step != 0);
    _binding.lbl2->setDisabled(_step != 1);
    _binding.lbl3->setDisabled(_step != 2);
    _binding.lbl4->setDisabled(_step != 3);
    _binding.lbl5->setDisabled(_step != 4);
    _binding.lbl6->setDisabled(_step != 5);
    _binding.btnStepName1->setDisabled(_step != 0);
    _binding.btnStepName2->setDisabled(_step != 1);
    _binding.btnStepName3->setDisabled(_step != 2);
    _binding.btnStepName4->setDisabled(_step != 3);
    _binding.btnStepName5->setDisabled(_step != 4);
    _binding.btnStepName6->setDisabled(_step != 5);

    _binding.lbl1->setFocus(_step == 0);
    _binding.lbl2->setFocus(_step == 1);
    _binding.lbl3->setFocus(_step == 2);
    _binding.lbl4->setFocus(_step == 3);
    _binding.lbl5->setFocus(_step == 4);
    _binding.lbl6->setFocus(_step == 5);
    _binding.lblNum1->setFocus(_step == 0);
    _binding.lblNum2->setFocus(_step == 1);
    _binding.lblNum3->setFocus(_step == 2);
    _binding.lblNum4->setFocus(_step == 3);
    _binding.lblNum5->setFocus(_step == 4);
    _binding.lblNum6->setFocus(_step == 5);
    _binding.btnStepName1->setFocus(_step == 0);
    _binding.btnStepName2->setFocus(_step == 1);
    _binding.btnStepName3->setFocus(_step == 2);
    _binding.btnStepName4->setFocus(_step == 3);
    _binding.btnStepName5->setFocus(_step == 4);
    _binding.btnStepName6->setFocus(_step == 5);
}

void CustomCharacterGeneration::goToNextStep() {
    if (_step < 6) {
        doSetStep(_step + 1);
    }
}

} // namespace game

} // namespace reone
