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

#include "reone/gui/control/button.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryinput.h"

#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void NameEntry::onGUILoaded() {
    bindControls();

    loadLtrFile("humanm", _maleLtr);
    loadLtrFile("humanf", _femaleLtr);
    loadLtrFile("humanl", _lastNameLtr);

    _binding.nameBoxEdit->setTextMessage("");

    _binding.btnRandom->setOnClick([this]() {
        loadRandomName();
    });
    _binding.endBtn->setOnClick([this]() {
        _charGen.goToNextStep();
        _charGen.openSteps();
    });
    _binding.btnBack->setOnClick([this]() {
        _charGen.openSteps();
    });
}

void NameEntry::bindControls() {
    _binding.btnBack = findControl<Button>("BTN_BACK");
    _binding.btnRandom = findControl<Button>("BTN_RANDOM");
    _binding.endBtn = findControl<Button>("END_BTN");
    _binding.nameBoxEdit = findControl<Control>("NAME_BOX_EDIT");
}

void NameEntry::loadLtrFile(const std::string &resRef, std::unique_ptr<LtrReader> &ltr) {
    auto data = _services.resource.resources.get(ResourceId(resRef, ResourceType::Ltr));
    auto stream = MemoryInputStream(*data);
    ltr = std::make_unique<LtrReader>(stream);
    ltr->load();
}

bool NameEntry::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && _input.handle(event)) {
        _binding.nameBoxEdit->setTextMessage(_input.text());
        return true;
    }
    return _gui->handle(event);
}

void NameEntry::loadRandomName() {
    _binding.nameBoxEdit->setTextMessage(getRandomName());
}

std::string NameEntry::getRandomName() const {
    Gender gender = _charGen.character().gender;
    auto &nameLtr = gender == Gender::Female ? _femaleLtr : _maleLtr;
    return nameLtr->getRandomName(8) + " " + _lastNameLtr->getRandomName(8);
}

} // namespace game

} // namespace reone
