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

#include "portraitselect.h"

#include "../../core/random.h"
#include "../../resource/resources.h"

#include "../types.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

PortraitSelectionGui::PortraitSelectionGui(GameVersion version, const GraphicsOptions &opts) : GUI(version, opts) {
    _resRef = getResRef("portcust");

    switch (version) {
        case GameVersion::TheSithLords:
            _resolutionX = 800;
            _resolutionY = 600;
            break;
        default:
            _backgroundType = BackgroundType::Menu;
            break;
    }
}

void PortraitSelectionGui::load() {
    GUI::load();

    setButtonColors("BTN_ACCEPT");
    setButtonColors("BTN_BACK");
}

void PortraitSelectionGui::setButtonColors(const string &tag) {
    Control &control = getControl(tag);

    Control::Text text(control.text());
    text.color = getBaseColor(_version);
    control.setText(move(text));

    Control::Border hilight(control.hilight());
    hilight.color = getHilightColor(_version);
    control.setHilight(move(hilight));
}

void PortraitSelectionGui::loadPortraits(const CreatureConfiguration &info) {
    if (!_portraits.empty() && info.gender == _character.gender) return;

    _character = info;

    shared_ptr<TwoDaTable> portraits(Resources.find2DA("portraits"));
    int sex = _character.gender == Gender::Female ? 1 : 0;

    _portraits.clear();
    for (auto &row : portraits->rows()) {
        if (row.getInt("forpc") == 1 && row.getInt("sex") == sex) {
            string resRef(row.getString("baseresref"));
            int appearanceNumber = row.getInt("appearancenumber");
            int appearanceS = row.getInt("appearance_s");
            int appearanceL = row.getInt("appearance_l");

            shared_ptr<Texture> image(Resources.findTexture(resRef, TextureType::GUI));

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

void PortraitSelectionGui::loadCurrentPortrait() {
    Control &control = getControl("LBL_PORTRAIT");

    Control::Border border(control.border());
    border.fill = _portraits[_currentPortrait].image;

    control.setBorder(move(border));
}

void PortraitSelectionGui::onClick(const string &control) {
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
            CreatureConfiguration charGenInfo(_character);
            charGenInfo.appearance = appearance;
            _onPortraitSelected(charGenInfo);
        }

    } else if (control == "BTN_BACK") {
        if (_onCancel) _onCancel();
    }
}

void PortraitSelectionGui::setOnPortraitSelected(const function<void(const CreatureConfiguration &)> &fn) {
    _onPortraitSelected = fn;
}

void PortraitSelectionGui::setOnCancel(const function<void()> &fn) {
    _onCancel = fn;
}

} // namespace game

} // namespace reone
