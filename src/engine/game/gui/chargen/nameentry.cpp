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

#include "nameentry.h"

#include "../../../common/streamutil.h"
#include "../../../resource/resources.h"

#include "../colorutil.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

NameEntry::NameEntry(CharacterGeneration *charGen, GameID gameId, const GraphicsOptions &opts) :
    GameGUI(gameId, opts),
    _charGen(charGen),
    _input(TextInputFlags::lettersWhitespace) {

    _resRef = getResRef("name");

    initForGame();
}

void NameEntry::load() {
    GUI::load();

    loadLtrFile("humanm", _maleLtr);
    loadLtrFile("humanf", _femaleLtr);
    loadLtrFile("humanl", _lastNameLtr);

    _nameBoxEdit = &getControl("NAME_BOX_EDIT");
    _nameBoxEdit->setTextMessage("");
}

void NameEntry::loadLtrFile(const string &resRef, LtrReader &ltr) {
    shared_ptr<ByteArray> data(Resources::instance().getRaw(resRef, ResourceType::Ltr));
    ltr.load(wrap(data));
}

bool NameEntry::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && _input.handle(event)) {
        _nameBoxEdit->setTextMessage(_input.text());
        return true;
    }
    return GUI::handle(event);
}

void NameEntry::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_RANDOM") {
        loadRandomName();

    } else if (control == "END_BTN") {
        _charGen->goToNextStep();
        _charGen->openSteps();

    } else if (control == "BTN_BACK") {
        _charGen->openSteps();
    }
}

void NameEntry::loadRandomName() {
    _nameBoxEdit->setTextMessage(getRandomName());
}

string NameEntry::getRandomName() const {
    Gender gender = _charGen->character().gender;
    const LtrReader &nameLtr = gender == Gender::Female ? _femaleLtr : _maleLtr;
    return nameLtr.getRandomName(8) + " " + _lastNameLtr.getRandomName(8);
}

} // namespace game

} // namespace reone
