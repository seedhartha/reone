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

#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"
#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void LevelUpMenu::onGUILoaded() {
    bindControls();

    doSetStep(0);

    _controls.BTN_BACK->setOnClick([this]() {
        _charGen.cancel();
    });
    _controls.BTN_STEPNAME1->setOnClick([this]() {
        _charGen.openAbilities();
    });
    _controls.BTN_STEPNAME2->setOnClick([this]() {
        _charGen.openSkills();
    });
    _controls.BTN_STEPNAME5->setOnClick([this]() {
        _charGen.finish();
    });
}

void LevelUpMenu::reset() {
    int nextLevel = _charGen.character().attributes.getAggregateLevel() + 1;
    _hasAttributes = nextLevel % 4 == 0;

    // TODO: feats and Force Powers are not yet implemented

    _controls.LBL_1->setVisible(_hasAttributes);
    _controls.LBL_3->setVisible(false);
    _controls.LBL_4->setVisible(false);
    _controls.LBL_NUM1->setVisible(_hasAttributes);
    _controls.LBL_NUM3->setVisible(false);
    _controls.LBL_NUM4->setVisible(false);
    _controls.BTN_STEPNAME1->setVisible(_hasAttributes);
    _controls.BTN_STEPNAME3->setVisible(false);
    _controls.BTN_STEPNAME4->setVisible(false);
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

    _controls.LBL_1->setSelectable(false);
    _controls.LBL_2->setSelectable(false);
    _controls.LBL_3->setSelectable(false);
    _controls.LBL_4->setSelectable(false);
    _controls.LBL_5->setSelectable(false);
    _controls.BTN_STEPNAME1->setSelectable(false);
    _controls.BTN_STEPNAME2->setSelectable(false);
    _controls.BTN_STEPNAME3->setSelectable(false);
    _controls.BTN_STEPNAME4->setSelectable(false);
    _controls.BTN_STEPNAME5->setSelectable(false);

    _controls.LBL_1->setDisabled(_step != 0);
    _controls.LBL_2->setDisabled(_step != 1);
    _controls.LBL_3->setDisabled(_step != 2);
    _controls.LBL_4->setDisabled(_step != 3);
    _controls.LBL_5->setDisabled(_step != 4);
    _controls.BTN_STEPNAME1->setDisabled(_step != 0);
    _controls.BTN_STEPNAME2->setDisabled(_step != 1);
    _controls.BTN_STEPNAME3->setDisabled(_step != 2);
    _controls.BTN_STEPNAME4->setDisabled(_step != 3);
    _controls.BTN_STEPNAME5->setDisabled(_step != 4);

    _controls.LBL_1->setSelected(_step == 0);
    _controls.LBL_2->setSelected(_step == 1);
    _controls.LBL_3->setSelected(_step == 2);
    _controls.LBL_4->setSelected(_step == 3);
    _controls.LBL_5->setSelected(_step == 4);
    _controls.LBL_NUM1->setSelected(_step == 0);
    _controls.LBL_NUM2->setSelected(_step == 1);
    _controls.LBL_NUM3->setSelected(_step == 2);
    _controls.LBL_NUM4->setSelected(_step == 3);
    _controls.LBL_NUM5->setSelected(_step == 4);
    _controls.BTN_STEPNAME1->setSelected(_step == 0);
    _controls.BTN_STEPNAME2->setSelected(_step == 1);
    _controls.BTN_STEPNAME3->setSelected(_step == 2);
    _controls.BTN_STEPNAME4->setSelected(_step == 3);
    _controls.BTN_STEPNAME5->setSelected(_step == 4);
}

} // namespace game

} // namespace reone
