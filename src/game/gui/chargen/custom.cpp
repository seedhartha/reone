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

#include "custom.h"

#include <boost/algorithm/string.hpp>

#include "../colorutil.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

CustomCharacterGeneration::CustomCharacterGeneration(CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &opts) :
    GUI(version, opts),
    _charGen(charGen) {

    _resRef = getResRef("custpnl");

    if (_version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
}

void CustomCharacterGeneration::load() {
    GUI::load();
    doSetStep(0);

    if (_version == GameVersion::KotOR) {
        configureControl("LBL_BG", [](Control &ctrl) { ctrl.setDiscardColor(glm::vec3(0.0f, 0.0f, 0.082353f)); });
    }
}

void CustomCharacterGeneration::setStep(int step) {
    if (_step != step) {
        doSetStep(step);
    }
}

void CustomCharacterGeneration::doSetStep(int step) {
    _step = step;

    setControlFocusable("LBL_1", false);
    setControlFocusable("LBL_2", false);
    setControlFocusable("LBL_3", false);
    setControlFocusable("LBL_4", false);
    setControlFocusable("LBL_5", false);
    setControlFocusable("LBL_6", false);
    setControlFocusable("BTN_STEPNAME1", false);
    setControlFocusable("BTN_STEPNAME2", false);
    setControlFocusable("BTN_STEPNAME3", false);
    setControlFocusable("BTN_STEPNAME4", false);
    setControlFocusable("BTN_STEPNAME5", false);
    setControlFocusable("BTN_STEPNAME6", false);

    setControlDisabled("LBL_1", _step != 0);
    setControlDisabled("LBL_2", _step != 1);
    setControlDisabled("LBL_3", _step != 2);
    setControlDisabled("LBL_4", _step != 3);
    setControlDisabled("LBL_5", _step != 4);
    setControlDisabled("LBL_6", _step != 5);
    setControlDisabled("BTN_STEPNAME1", _step != 0);
    setControlDisabled("BTN_STEPNAME2", _step != 1);
    setControlDisabled("BTN_STEPNAME3", _step != 2);
    setControlDisabled("BTN_STEPNAME4", _step != 3);
    setControlDisabled("BTN_STEPNAME5", _step != 4);
    setControlDisabled("BTN_STEPNAME6", _step != 5);

    setControlFocus("LBL_1", _step == 0);
    setControlFocus("LBL_2", _step == 1);
    setControlFocus("LBL_3", _step == 2);
    setControlFocus("LBL_4", _step == 3);
    setControlFocus("LBL_5", _step == 4);
    setControlFocus("LBL_6", _step == 5);
    setControlFocus("LBL_NUM1", _step == 0);
    setControlFocus("LBL_NUM2", _step == 1);
    setControlFocus("LBL_NUM3", _step == 2);
    setControlFocus("LBL_NUM4", _step == 3);
    setControlFocus("LBL_NUM5", _step == 4);
    setControlFocus("LBL_NUM6", _step == 5);
    setControlFocus("BTN_STEPNAME1", _step == 0);
    setControlFocus("BTN_STEPNAME2", _step == 1);
    setControlFocus("BTN_STEPNAME3", _step == 2);
    setControlFocus("BTN_STEPNAME4", _step == 3);
    setControlFocus("BTN_STEPNAME5", _step == 4);
    setControlFocus("BTN_STEPNAME6", _step == 5);
}

void CustomCharacterGeneration::goToNextStep() {
    if (_step < 6) {
        doSetStep(_step + 1);
    }
}

void CustomCharacterGeneration::onClick(const string &control) {
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
                _charGen->openAbilities();
                break;
            case 3:
                _charGen->openSkills();
                break;
            case 4:
                _charGen->openFeats();
                break;
            case 5:
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
