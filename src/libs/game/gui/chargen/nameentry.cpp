/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/gui/chargen/nameentry.h"

#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"
#include "reone/gui/control/button.h"
#include "reone/resource/provider/ltrs.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static const ResRef kMaleNameLtrResRef("humanm");
static const ResRef kFemaleNameLtrResRef("humanf");
static const ResRef kLastNameLtrResRef("humanl");

void NameEntry::onGUILoaded() {
    bindControls();

    _controls.NAME_BOX_EDIT->setTextMessage("");

    _controls.BTN_RANDOM->setOnClick([this]() {
        loadRandomName();
    });
    _controls.END_BTN->setOnClick([this]() {
        _charGen.goToNextStep();
        _charGen.openSteps();
    });
    _controls.BTN_BACK->setOnClick([this]() {
        _charGen.openSteps();
    });
}

bool NameEntry::handle(const input::Event &event) {
    if (event.type == input::EventType::KeyDown && _input.handle(event)) {
        _controls.NAME_BOX_EDIT->setTextMessage(_input.text());
        return true;
    }
    return _gui->handle(event);
}

void NameEntry::loadRandomName() {
    auto gender = _charGen.character().gender;
    const auto &nameLtrResRef = (gender == Gender::Female) ? kFemaleNameLtrResRef : kMaleNameLtrResRef;
    auto nameLtr = _services.resource.ltrs.get(nameLtrResRef);
    auto lastNameLtr = _services.resource.ltrs.get(kLastNameLtrResRef);
    auto generated = nameLtr->randomName(8) + " " + lastNameLtr->randomName(8);
    _controls.NAME_BOX_EDIT->setTextMessage(generated);
}

} // namespace game

} // namespace reone
