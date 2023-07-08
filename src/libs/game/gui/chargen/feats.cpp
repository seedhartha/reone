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

#include "reone/game/gui/chargen/feats.h"

#include "reone/gui/control/button.h"

#include "reone/game/game.h"

#include "reone/game/gui/chargen.h"

using namespace reone::audio;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void CharGenFeats::onGUILoaded() {
    bindControls();

    _binding.btnAccept->setOnClick([this]() {
        _charGen.goToNextStep();
        _charGen.openSteps();
    });
    _binding.btnBack->setOnClick([this]() {
        _charGen.openSteps();
    });
    _binding.btnSelect->setDisabled(true);
    _binding.btnRecommended->setDisabled(true);
}

void CharGenFeats::bindControls() {
    _binding.btnAccept = findControl<Button>("BTN_ACCEPT");
    _binding.btnBack = findControl<Button>("BTN_BACK");
    _binding.btnSelect = findControl<Button>("BTN_SELECT");
    _binding.btnRecommended = findControl<Button>("BTN_RECOMMENDED");
}

} // namespace game

} // namespace reone
