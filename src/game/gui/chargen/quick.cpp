/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../../types.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

QuickCharacterGeneration::QuickCharacterGeneration(GameVersion version, const GraphicsOptions &opts) : GUI(version, opts) {
    _resRef = getResRef("quickpnl");

    if (_version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void QuickCharacterGeneration::load() {
    GUI::load();
    setStep(0);
}

void QuickCharacterGeneration::setStep(int step) {
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

    glm::vec3 hilightColor(getHilightColor(_version));
    setControlHilightColor("LBL_NUM1", hilightColor);
    setControlHilightColor("LBL_NUM2", hilightColor);
    setControlHilightColor("LBL_NUM3", hilightColor);
    setControlHilightColor("BTN_STEPNAME1", hilightColor);
    setControlHilightColor("BTN_STEPNAME2", hilightColor);
    setControlHilightColor("BTN_STEPNAME3", hilightColor);
    setControlHilightColor("BTN_BACK", hilightColor);
    setControlHilightColor("BTN_CANCEL", hilightColor);
}

void QuickCharacterGeneration::onClick(const string &control) {
    if (control == "BTN_CANCEL") {
        setStep(0);

        if (_onCancel) {
            _onCancel();
        }
    } else if (control == "BTN_BACK") {
        if (_step == 0) {
            if (_onCancel) {
                _onCancel();
            }
        } else {
            setStep(_step - 1);
        }
    } else if (boost::starts_with(control, "BTN_STEPNAME")) {
        int step = control[12] - '0';
        if (_onStepSelected) {
            _onStepSelected(step);
        }
    }
}

void QuickCharacterGeneration::setOnStepSelected(const function<void(int)> &fn) {
    _onStepSelected = fn;
}

void QuickCharacterGeneration::setOnCancel(const function<void()> &fn) {
    _onCancel = fn;
}

} // namespace game

} // namespace reone
