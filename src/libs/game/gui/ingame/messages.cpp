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

#include "reone/game/gui/ingame/messages.h"

#include "reone/gui/control/button.h"

#include "reone/game/game.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void MessagesMenu::onGUILoaded() {
    loadBackground(BackgroundType::Menu);
    bindControls();

    _binding.btnExit->setOnClick([this]() {
        _game.openInGame();
    });

    if (!_game.isTSL()) {
        _binding.btnShow->setDisabled(true);
    }
}

void MessagesMenu::bindControls() {
    _binding.btnExit = findControl<Button>("BTN_EXIT");

    if (!_game.isTSL()) {
        _binding.btnShow = findControl<Button>("BTN_SHOW");
    }
}

} // namespace game

} // namespace reone
