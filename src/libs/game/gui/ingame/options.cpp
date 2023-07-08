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

#include "reone/game/gui/ingame/options.h"

#include "reone/gui/control/button.h"

#include "reone/game/game.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void OptionsMenu::onGUILoaded() {
    loadBackground(BackgroundType::Menu);
    bindControls();

    _binding.btnLoadGame->setOnClick([this]() {
        _game.openSaveLoad(SaveLoadMode::LoadFromInGame);
    });
    _binding.btnSaveGame->setOnClick([this]() {
        _game.openSaveLoad(SaveLoadMode::Save);
    });
    _binding.btnExit->setOnClick([this]() {
        _game.openInGame();
    });
}

void OptionsMenu::bindControls() {
    _binding.btnLoadGame = findControl<Button>("BTN_LOADGAME");
    _binding.btnSaveGame = findControl<Button>("BTN_SAVEGAME");
    _binding.btnExit = findControl<Button>("BTN_EXIT");
}

} // namespace game

} // namespace reone
