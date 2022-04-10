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

#include "messages.h"

#include "../../../../gui/control/button.h"

#include "../../kotor.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace kotor {

MessagesMenu::MessagesMenu(KotOR &game, GameServices &services) :
    GameGUI(game, services) {
    _resRef = getResRef("messages");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void MessagesMenu::load() {
    GUI::load();
    bindControls();

    _binding.btnExit->setOnClick([this]() {
        _game.openInGame();
    });

    if (!_game.isTSL()) {
        _binding.btnShow->setDisabled(true);
    }
}

void MessagesMenu::bindControls() {
    _binding.btnExit = getControl<Button>("BTN_EXIT");

    if (!_game.isTSL()) {
        _binding.btnShow = getControl<Button>("BTN_SHOW");
    }
}

} // namespace kotor

} // namespace reone
