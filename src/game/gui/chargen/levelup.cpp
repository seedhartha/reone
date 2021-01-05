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

#include "levelup.h"

#include "../../game.h"

#include "../colorutil.h"

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

LevelUpMenu::LevelUpMenu(CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &options) :
    GameGUI(version, options),
    _charGen(charGen) {

    _resRef = getResRef("leveluppnl");

    initForGame();
}

void LevelUpMenu::load() {
    GUI::load();
    doSetStep(0);

    if (_version == GameVersion::KotOR) {
        setControlDiscardColor("LBL_DECORATION", glm::vec3(0.0f, 0.0f, 0.082353f));
    }
}

void LevelUpMenu::goToNextStep() {
    if (_step < 5) {
        doSetStep(_step + 1);
    }
}

void LevelUpMenu::setStep(int step) {
    if (_step != step) {
        doSetStep(step);
    }
}

void LevelUpMenu::doSetStep(int step) {
    _step = step;

    setControlFocusable("LBL_1", false);
    setControlFocusable("LBL_2", false);
    setControlFocusable("LBL_3", false);
    setControlFocusable("LBL_4", false);
    setControlFocusable("LBL_5", false);
    setControlFocusable("BTN_STEPNAME1", false);
    setControlFocusable("BTN_STEPNAME2", false);
    setControlFocusable("BTN_STEPNAME3", false);
    setControlFocusable("BTN_STEPNAME4", false);
    setControlFocusable("BTN_STEPNAME5", false);

    setControlDisabled("LBL_1", _step != 0);
    setControlDisabled("LBL_2", _step != 1);
    setControlDisabled("LBL_3", _step != 2);
    setControlDisabled("LBL_4", _step != 3);
    setControlDisabled("LBL_5", _step != 4);
    setControlDisabled("BTN_STEPNAME1", _step != 0);
    setControlDisabled("BTN_STEPNAME2", _step != 1);
    setControlDisabled("BTN_STEPNAME3", _step != 2);
    setControlDisabled("BTN_STEPNAME4", _step != 3);
    setControlDisabled("BTN_STEPNAME5", _step != 4);

    setControlFocus("LBL_1", _step == 0);
    setControlFocus("LBL_2", _step == 1);
    setControlFocus("LBL_3", _step == 2);
    setControlFocus("LBL_4", _step == 3);
    setControlFocus("LBL_5", _step == 4);
    setControlFocus("LBL_NUM1", _step == 0);
    setControlFocus("LBL_NUM2", _step == 1);
    setControlFocus("LBL_NUM3", _step == 2);
    setControlFocus("LBL_NUM4", _step == 3);
    setControlFocus("LBL_NUM5", _step == 4);
    setControlFocus("BTN_STEPNAME1", _step == 0);
    setControlFocus("BTN_STEPNAME2", _step == 1);
    setControlFocus("BTN_STEPNAME3", _step == 2);
    setControlFocus("BTN_STEPNAME4", _step == 3);
    setControlFocus("BTN_STEPNAME5", _step == 4);
}

void LevelUpMenu::onClick(const std::string &control) {
    if (control == "BTN_BACK") {
        _charGen->cancel();
    }
}

} // namespace game

} // namespace reone
