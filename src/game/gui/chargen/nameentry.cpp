/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "nameentry.h"

#include "../colors.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

NameEntry::NameEntry(CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &opts) :
    GUI(version, opts),
    _charGen(charGen),
    _input(kTextInputLetters | kTextInputWhitespace) {

    _resRef = getResRef("name");

    if (version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
}

void NameEntry::load() {
    GUI::load();

    setControlText("NAME_BOX_EDIT", "");
    disableControl("BTN_RANDOM");
}

bool NameEntry::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && _input.handle(event)) {
        setControlText("NAME_BOX_EDIT", _input.text());
        return true;
    }

    return GUI::handle(event);
}

void NameEntry::onClick(const string &control) {
    if (control == "END_BTN") {
        _charGen->setQuickStep(2);
        _charGen->openQuick();

    } else if (control == "BTN_BACK") {
        _charGen->openQuick();
    }
}

} // namespace game

} // namespace reone
