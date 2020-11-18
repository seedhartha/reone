/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../gui/gui.h"

namespace reone {

namespace game {

class Game;

class SaveLoad : public gui::GUI {
public:
    enum class Mode {
        Save,
        LoadFromMainMenu,
        LoadFromInGame
    };

    SaveLoad(Game *game);

    void load() override;

    void update();

    void setMode(Mode mode);

private:
    struct GameDescriptor {
        int index { 0 };
        std::string name;
        boost::filesystem::path path;
    };

    Game *_game { nullptr };
    Mode _mode { Mode::Save };
    std::vector<GameDescriptor> _saves;
    int _selectedSaveIdx { -1 };

    void onClick(const std::string &control) override;

    void indexSavedGames();
    void indexSavedGame(int index, const boost::filesystem::path &path);

    void saveGame(int index);
    void loadGame(int index);
    void deleteGame(int index);

    boost::filesystem::path getSavesPath() const;
    boost::filesystem::path getSaveDirPath(int index) const;
    std::string getSaveName(int index) const;
    int getSelectedSaveIndex() const;
    int getNewSaveIndex() const;
};

} // namespace game

} // namespace reone
