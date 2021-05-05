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

#pragma once

#include <boost/filesystem/path.hpp>

#include "../savedgame.h"

#include "gui.h"

namespace reone {

namespace game {

class Game;

class SaveLoad : public GameGUI {
public:
    enum class Mode {
        Save,
        LoadFromMainMenu,
        LoadFromInGame
    };

    SaveLoad(Game *game);

    void load() override;

    void refresh();

    void setMode(Mode mode);

private:
    struct SavedGameDescriptor {
        int number { 0 };
        SavedGame save;
        boost::filesystem::path path;
    };

    Game *_game { nullptr };
    Mode _mode { Mode::Save };
    std::vector<SavedGameDescriptor> _saves;

    void refreshSavedGames();
    void indexSavedGame(boost::filesystem::path path);

    void onClick(const std::string &control) override;

    void saveGame(int number);
    void loadGame(int number);
    void deleteGame(int number);

    int getSelectedSaveNumber() const;
    int getNewSaveNumber() const;
};

} // namespace game

} // namespace reone
