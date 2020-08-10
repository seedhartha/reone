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

#include "portraits.h"

#include "../../core/random.h"
#include "../../resources/manager.h"

#include "../types.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

PortraitsGui::PortraitsGui(const GraphicsOptions &opts) : GUI(opts) {
}

void PortraitsGui::load(GameVersion version) {
    string resRef;
    BackgroundType background;

    switch (version) {
        case GameVersion::TheSithLords:
            _resolutionX = 800;
            _resolutionY = 600;
            resRef = "portcust_p";
            background = BackgroundType::None;
            break;
        default:
            resRef = "portcust";
            background = BackgroundType::Menu;
            break;
    }

    GUI::load(resRef, background);
    _version = version;

    setButtonColors("BTN_ACCEPT");
    setButtonColors("BTN_BACK");
}

void PortraitsGui::setButtonColors(const string &tag) {
    Control &control = getControl(tag);

    Control::Text text(control.text());
    text.color = _version == GameVersion::KotOR ? getKotorBaseColor() : getTslBaseColor();
    control.setText(move(text));

    Control::Border hilight(control.hilight());
    hilight.color = _version == GameVersion::KotOR ? getKotorHilightColor() : getTslHilightColor();
    control.setHilight(move(hilight));
}

void PortraitsGui::loadPortraits(const CharacterConfiguration &info) {
    if (!_portraits.empty() && info.gender == _character.gender) return;

    _character = info;
    string sex = to_string(_character.gender == Gender::Female ? 1 : 0);

    shared_ptr<TwoDaTable> portraits(ResMan.find2DA("portraits"));
    int baseResRefIdx = portraits->getColumnIndex("baseresref");
    int sexIdx = portraits->getColumnIndex("sex");
    int forPcIdx = portraits->getColumnIndex("forpc");
    int appearanceNumberIdx = portraits->getColumnIndex("appearancenumber");
    int appearanceSIdx = portraits->getColumnIndex("appearance_s");
    int appearanceLIdx = portraits->getColumnIndex("appearance_l");

    _portraits.clear();
    for (auto &row : portraits->rows()) {
        if (row.values[forPcIdx] == "1" && row.values[sexIdx] == sex) {
            string resRef(row.values[baseResRefIdx]);
            int appearanceNumber(stoi(row.values[appearanceNumberIdx]));
            int appearanceS(stoi(row.values[appearanceSIdx]));
            int appearanceL(stoi(row.values[appearanceLIdx]));

            shared_ptr<Texture> image(ResMan.findTexture(resRef, TextureType::GUI));
            image->initGL();

            Portrait portrait;
            portrait.resRef = move(resRef);
            portrait.image = move(image);
            portrait.appearanceNumber = appearanceNumber;
            portrait.appearanceS = appearanceS;
            portrait.appearanceL = appearanceL;

            _portraits.push_back(move(portrait));
        }
    }

    _currentPortrait = random(0, static_cast<int>(_portraits.size()) - 1);
    loadCurrentPortrait();
}

void PortraitsGui::loadCurrentPortrait() {
    Control &control = getControl("LBL_PORTRAIT");

    Control::Border border(control.border());
    border.fill = _portraits[_currentPortrait].image;

    control.setBorder(move(border));
}

void PortraitsGui::onClick(const string &control) {
    int portraitCount = static_cast<int>(_portraits.size());

    if (control == "BTN_ARRL") {
        _currentPortrait--;
        if (_currentPortrait == -1) {
            _currentPortrait = portraitCount - 1;
        }
        loadCurrentPortrait();

    } else if (control == "BTN_ARRR") {
        _currentPortrait = ++_currentPortrait % portraitCount;
        loadCurrentPortrait();

    } else if (control == "BTN_ACCEPT") {
        if (_onPortraitSelected) {
            int appearance;
            switch (_character.clazz) {
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
            CharacterConfiguration charGenInfo(_character);
            charGenInfo.appearance = appearance;
            _onPortraitSelected(charGenInfo);
        }

    } else if (control == "BTN_BACK") {
        if (_onCancel) _onCancel();
    }
}

void PortraitsGui::setOnPortraitSelected(const function<void(const CharacterConfiguration &)> &fn) {
    _onPortraitSelected = fn;
}

void PortraitsGui::setOnCancel(const function<void()> &fn) {
    _onCancel = fn;
}

} // namespace game

} // namespace reone
