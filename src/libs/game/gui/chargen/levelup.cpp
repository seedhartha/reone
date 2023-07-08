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

#include "reone/game/gui/chargen/levelup.h"

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

void LevelUpMenu::onGUILoaded() {
    bindControls();

    doSetStep(0);

    _binding.btnBack->setOnClick([this]() {
        _charGen.cancel();
    });
    _binding.btnStepName1->setOnClick([this]() {
        _charGen.openAbilities();
    });
    _binding.btnStepName2->setOnClick([this]() {
        _charGen.openSkills();
    });
    _binding.btnStepName5->setOnClick([this]() {
        _charGen.finish();
    });
}

void LevelUpMenu::bindControls() {
    _binding.btnBack = findControl<Button>("BTN_BACK");
    _binding.btnStepName1 = findControl<Button>("BTN_STEPNAME1");
    _binding.btnStepName2 = findControl<Button>("BTN_STEPNAME2");
    _binding.btnStepName3 = findControl<Button>("BTN_STEPNAME3");
    _binding.btnStepName4 = findControl<Button>("BTN_STEPNAME4");
    _binding.btnStepName5 = findControl<Button>("BTN_STEPNAME5");
    _binding.lbl1 = findControl<Label>("LBL_1");
    _binding.lbl2 = findControl<Label>("LBL_2");
    _binding.lbl3 = findControl<Label>("LBL_3");
    _binding.lbl4 = findControl<Label>("LBL_4");
    _binding.lbl5 = findControl<Label>("LBL_5");
    _binding.lblNum1 = findControl<Label>("LBL_NUM1");
    _binding.lblNum2 = findControl<Label>("LBL_NUM2");
    _binding.lblNum3 = findControl<Label>("LBL_NUM3");
    _binding.lblNum4 = findControl<Label>("LBL_NUM4");
    _binding.lblNum5 = findControl<Label>("LBL_NUM5");
}

void LevelUpMenu::reset() {
    int nextLevel = _charGen.character().attributes.getAggregateLevel() + 1;
    _hasAttributes = nextLevel % 4 == 0;

    // TODO: feats and Force Powers are not yet implemented

    _binding.lbl1->setVisible(_hasAttributes);
    _binding.lbl3->setVisible(false);
    _binding.lbl4->setVisible(false);
    _binding.lblNum1->setVisible(_hasAttributes);
    _binding.lblNum3->setVisible(false);
    _binding.lblNum4->setVisible(false);
    _binding.btnStepName1->setVisible(_hasAttributes);
    _binding.btnStepName3->setVisible(false);
    _binding.btnStepName4->setVisible(false);
}

void LevelUpMenu::goToNextStep() {
    switch (_step) {
    case 0:
        doSetStep(1);
        break;
    case 1:
        doSetStep(4);
        break;
    default:
        break;
    }
}

void LevelUpMenu::setStep(int step) {
    if (_step != step) {
        doSetStep(step);
    }
}

void LevelUpMenu::doSetStep(int step) {
    _step = step;

    _binding.lbl1->setFocusable(false);
    _binding.lbl2->setFocusable(false);
    _binding.lbl3->setFocusable(false);
    _binding.lbl4->setFocusable(false);
    _binding.lbl5->setFocusable(false);
    _binding.btnStepName1->setFocusable(false);
    _binding.btnStepName2->setFocusable(false);
    _binding.btnStepName3->setFocusable(false);
    _binding.btnStepName4->setFocusable(false);
    _binding.btnStepName5->setFocusable(false);

    _binding.lbl1->setDisabled(_step != 0);
    _binding.lbl2->setDisabled(_step != 1);
    _binding.lbl3->setDisabled(_step != 2);
    _binding.lbl4->setDisabled(_step != 3);
    _binding.lbl5->setDisabled(_step != 4);
    _binding.btnStepName1->setDisabled(_step != 0);
    _binding.btnStepName2->setDisabled(_step != 1);
    _binding.btnStepName3->setDisabled(_step != 2);
    _binding.btnStepName4->setDisabled(_step != 3);
    _binding.btnStepName5->setDisabled(_step != 4);

    _binding.lbl1->setFocus(_step == 0);
    _binding.lbl2->setFocus(_step == 1);
    _binding.lbl3->setFocus(_step == 2);
    _binding.lbl4->setFocus(_step == 3);
    _binding.lbl5->setFocus(_step == 4);
    _binding.lblNum1->setFocus(_step == 0);
    _binding.lblNum2->setFocus(_step == 1);
    _binding.lblNum3->setFocus(_step == 2);
    _binding.lblNum4->setFocus(_step == 3);
    _binding.lblNum5->setFocus(_step == 4);
    _binding.btnStepName1->setFocus(_step == 0);
    _binding.btnStepName2->setFocus(_step == 1);
    _binding.btnStepName3->setFocus(_step == 2);
    _binding.btnStepName4->setFocus(_step == 3);
    _binding.btnStepName5->setFocus(_step == 4);
}

} // namespace game

} // namespace reone
