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

#include "game.h"

#include "../common/pathutil.h"

namespace fs = boost::filesystem;

namespace reone {

namespace game {

GameID Game::determineGameID(const fs::path &gameDir) const {
    // If there is no swkotor2 executable, then this is KotOR
    fs::path exePath(getPathIgnoreCase(gameDir, "swkotor2.exe", false));
    if (exePath.empty()) return GameID::KotOR;

    // If there is a "steam_api.dll" file, then this is a Steam version of TSL
    fs::path dllPath(getPathIgnoreCase(gameDir, "steam_api.dll", false));
    if (!dllPath.empty()) return GameID::TSL_Steam;

    return GameID::TSL;
}

bool Game::isKotOR() const {
    return _gameId == GameID::KotOR;
}

bool Game::isTSL() const {
    switch (_gameId) {
        case GameID::TSL:
        case GameID::TSL_Steam:
            return true;
        default:
            return false;
    }
}

} // namespace game

} // namespace reone
