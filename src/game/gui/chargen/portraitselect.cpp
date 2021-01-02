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

#include "portraitselect.h"

#include "../../../common/random.h"
#include "../../../render/textures.h"
#include "../../../resource/resources.h"

#include "../colorutil.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

PortraitSelection::PortraitSelection(CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &opts) :
    GUI(version, opts),
    _charGen(charGen) {

    _resRef = getResRef("portcust");

    switch (version) {
        case GameVersion::TheSithLords:
            _resolutionX = 800;
            _resolutionY = 600;
            break;
        default:
            _backgroundType = BackgroundType::Menu;
            _hasDefaultHilightColor = true;
            _defaultHilightColor = getHilightColor(_version);
            break;
    }
}

void PortraitSelection::load() {
    GUI::load();

    setButtonColors("BTN_ACCEPT");
    setButtonColors("BTN_BACK");
}

void PortraitSelection::setButtonColors(const string &tag) {
    Control &control = getControl(tag);

    Control::Text text(control.text());
    text.color = getBaseColor(_version);
    control.setText(move(text));

    Control::Border hilight(control.hilight());
    hilight.color = getHilightColor(_version);
    control.setHilight(move(hilight));
}

void PortraitSelection::updatePortraits() {
    _portraits.clear();

    shared_ptr<TwoDaTable> portraits(Resources::instance().get2DA("portraits"));
    const CreatureConfiguration &character = _charGen->character();
    int sex = character.gender == Gender::Female ? 1 : 0;

    for (auto &row : portraits->rows()) {
        if (row.getInt("forpc") == 1 && row.getInt("sex") == sex) {
            string resRef(row.getString("baseresref"));
            int appearanceNumber = row.getInt("appearancenumber");
            int appearanceS = row.getInt("appearance_s");
            int appearanceL = row.getInt("appearance_l");

            shared_ptr<Texture> image(Textures::instance().get(resRef, TextureType::GUI));

            Portrait portrait;
            portrait.resRef = move(resRef);
            portrait.image = move(image);
            portrait.appearanceNumber = appearanceNumber;
            portrait.appearanceS = appearanceS;
            portrait.appearanceL = appearanceL;

            _portraits.push_back(move(portrait));
        }
    }
    resetCurrentPortrait();
}

void PortraitSelection::resetCurrentPortrait() {
    const CreatureConfiguration &character = _charGen->character();
    auto maybePortrait = find_if(_portraits.begin(), _portraits.end(), [&character](const Portrait &portrait) {
        return
            portrait.appearanceNumber == character.appearance ||
            portrait.appearanceS == character.appearance ||
            portrait.appearanceL == character.appearance;
    });
    _currentPortrait = static_cast<int>(distance(_portraits.begin(), maybePortrait));
    loadCurrentPortrait();
}

void PortraitSelection::loadCurrentPortrait() {
    Control &control = getControl("LBL_PORTRAIT");

    Control::Border border(control.border());
    border.fill = _portraits[_currentPortrait].image;

    control.setBorder(move(border));
}

void PortraitSelection::onClick(const string &control) {
    int portraitCount = static_cast<int>(_portraits.size());

    if (control == "BTN_ARRL") {
        _currentPortrait--;
        if (_currentPortrait == -1) {
            _currentPortrait = portraitCount - 1;
        }
        loadCurrentPortrait();

    } else if (control == "BTN_ARRR") {
        _currentPortrait = (_currentPortrait + 1) % portraitCount;
        loadCurrentPortrait();

    } else if (control == "BTN_ACCEPT") {
        CreatureConfiguration character(_charGen->character());
        int appearance;
        switch (character.clazz) {
            case ClassType::Scoundrel:
                appearance = _portraits[_currentPortrait].appearanceS;
                break;
            case ClassType::Soldier:
                appearance = _portraits[_currentPortrait].appearanceL;
                break;
            default:
                appearance = _portraits[_currentPortrait].appearanceNumber;
                break;
        }
        character.appearance = appearance;
        _charGen->setCharacter(character);
        _charGen->goToNextStep();
        _charGen->openSteps();

    } else if (control == "BTN_BACK") {
        resetCurrentPortrait();
        _charGen->openSteps();
    }
}

} // namespace game

} // namespace reone
