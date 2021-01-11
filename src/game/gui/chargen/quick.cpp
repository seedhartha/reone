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

#include <boost/algorithm/string.hpp>

#include "../colorutil.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

QuickCharacterGeneration::QuickCharacterGeneration(CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &opts) :
    GameGUI(version, opts),
    _charGen(charGen) {

    _resRef = getResRef("quickpnl");

    initForGame();
}

void QuickCharacterGeneration::load() {
    GUI::load();
    doSetStep(0);

    if (_version == GameVersion::KotOR) {
        setControlDiscardColor("LBL_DECORATION", glm::vec3(0.0f, 0.0f, 0.082353f));
    }
}

void QuickCharacterGeneration::setStep(int step) {
    if (_step != step) {
        doSetStep(step);
    }
}

void QuickCharacterGeneration::doSetStep(int step) {
    _step = step;

    setControlFocusable("LBL_1", false);
    setControlFocusable("LBL_2", false);
    setControlFocusable("LBL_3", false);
    setControlFocusable("BTN_STEPNAME1", false);
    setControlFocusable("BTN_STEPNAME2", false);
    setControlFocusable("BTN_STEPNAME3", false);

    setControlDisabled("LBL_1", _step != 0);
    setControlDisabled("LBL_2", _step != 1);
    setControlDisabled("LBL_3", _step != 2);
    setControlDisabled("BTN_STEPNAME1", _step != 0);
    setControlDisabled("BTN_STEPNAME2", _step != 1);
    setControlDisabled("BTN_STEPNAME3", _step != 2);

    setControlFocus("LBL_1", _step == 0);
    setControlFocus("LBL_2", _step == 1);
    setControlFocus("LBL_3", _step == 2);
    setControlFocus("LBL_NUM1", _step == 0);
    setControlFocus("LBL_NUM2", _step == 1);
    setControlFocus("LBL_NUM3", _step == 2);
    setControlFocus("BTN_STEPNAME1", _step == 0);
    setControlFocus("BTN_STEPNAME2", _step == 1);
    setControlFocus("BTN_STEPNAME3", _step == 2);
}

void QuickCharacterGeneration::goToNextStep() {
    if (_step < 3) {
        doSetStep(_step + 1);
    }
}

void QuickCharacterGeneration::onClick(const string &control) {
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
