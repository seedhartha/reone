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

#include "journal.h"

#include "../../game.h"

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

JournalMenu::JournalMenu(Game *game) :
    GameGUI(game) {
    _resRef = getResRef("journal");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void JournalMenu::load() {
    GUI::load();
    bindControls();

    _binding.btnExit->setOnClick([this]() {
        _game->openInGame();
    });
    _binding.btnSwapText->setDisabled(true);

    if (_game->isKotOR()) {
        _binding.btnQuestItems->setDisabled(true);
        _binding.btnSort->setDisabled(true);
    }
}

void JournalMenu::bindControls() {
    _binding.btnExit = getControl<Button>("BTN_EXIT");
    _binding.btnSwapText = getControl<Button>("BTN_SWAPTEXT");

    if (_game->isKotOR()) {
        _binding.btnQuestItems = getControl<Button>("BTN_QUESTITEMS");
        _binding.btnSort = getControl<Button>("BTN_SORT");
    }
}

} // namespace game

} // namespace reone
