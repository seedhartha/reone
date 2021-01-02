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

#include "skills.h"

#include "../colorutil.h"

#include "chargen.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

CharGenSkills::CharGenSkills(CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &opts) :
    GUI(version, opts),
    _charGen(charGen) {

    _resRef = getResRef("skchrgen");

    if (version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
}

void CharGenSkills::load() {
    GUI::load();

    disableControl("COMPUTER_USE_POINTS_BTN");
    disableControl("DEMOLITIONS_POINTS_BTN");
    disableControl("STEALTH_POINTS_BTN");
    disableControl("AWARENESS_POINTS_BTN");
    disableControl("PERSUADE_POINTS_BTN");
    disableControl("REP_MINUS_BTN");
    disableControl("SECURITY_POINTS_BTN");
    disableControl("TREAT_INJURY_POINTS_BTN");

    disableControl("BTN_RECOMMENDED");
}

void CharGenSkills::onClick(const string &control) {
    if (control == "BTN_ACCEPT") {
        _charGen->goToNextStep();
        _charGen->openSteps();
    } else if (control == "BTN_BACK") {
        _charGen->openSteps();
    }
}

} // namespace game

} // namespace reone
