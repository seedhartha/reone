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

#include <boost/algorithm/string.hpp>

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

void LevelUpMenu::reset() {
    const StaticCreatureBlueprint &character = _charGen->character();
    int nextLevel = character.attributes().getAggregateLevel() + 1;
    _hasAttributes = nextLevel  % 4 == 0;

    // TODO: feats and Force Powers are not yet implemented

    setControlVisible("LBL_1", _hasAttributes);
    setControlVisible("LBL_3", false);
    setControlVisible("LBL_4", false);
    setControlVisible("LBL_NUM1", _hasAttributes);
    setControlVisible("LBL_NUM3", false);
    setControlVisible("LBL_NUM4", false);
    setControlVisible("BTN_STEPNAME1", _hasAttributes);
    setControlVisible("BTN_STEPNAME3", false);
    setControlVisible("BTN_STEPNAME4", false);
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
    } else if (boost::starts_with(control, "BTN_STEPNAME")) {
        int step = control[12] - '0';
        switch (step) {
            case 1:
                _charGen->openAbilities();
                break;
            case 2:
                _charGen->openSkills();
                break;
            default:
                _charGen->finish();
                break;
        }
    }
}

} // namespace game

} // namespace reone
