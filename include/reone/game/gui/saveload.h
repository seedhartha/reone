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

#pragma once

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"

#include "../gui.h"
#include "../savedgame.h"

namespace reone {

namespace game {

class SaveLoad : public GameGUI {
public:
    SaveLoad(Game &game, ServicesView &services) :
        GameGUI(game, services) {
        _resRef = guiResRef("saveload");
    }

    void refresh();

    void setMode(SaveLoadMode mode);

private:
    struct SavedGameDescriptor {
        int number {0};
        SavedGame save;
        std::filesystem::path path;
    };

    struct Binding {
        std::shared_ptr<gui::Button> btnBack;
        std::shared_ptr<gui::Button> btnDelete;
        std::shared_ptr<gui::Button> btnSaveLoad;
        std::shared_ptr<gui::Label> lblAreaName;
        std::shared_ptr<gui::Label> lblPanelName;
        std::shared_ptr<gui::Label> lblPlanetName;
        std::shared_ptr<gui::Label> lblPm1;
        std::shared_ptr<gui::Label> lblPm2;
        std::shared_ptr<gui::Label> lblPm3;
        std::shared_ptr<gui::Label> lblScreenshot;
        std::shared_ptr<gui::ListBox> lbGames;

        // TSL only
        std::shared_ptr<gui::Button> btnFilter;
        std::shared_ptr<gui::Label> lblBar1;
        std::shared_ptr<gui::Label> lblBar2;
        std::shared_ptr<gui::Label> lblBar3;
        std::shared_ptr<gui::Label> lblBar4;
        std::shared_ptr<gui::Label> lblPcName;
        std::shared_ptr<gui::Label> lblTimePlayed;
        // END TSL only
    } _binding;

    SaveLoadMode _mode {SaveLoadMode::Save};
    std::vector<SavedGameDescriptor> _saves;

    void onGUILoaded() override;

    void bindControls();
    void refreshSavedGames();
    void indexSavedGame(std::filesystem::path path);

    void saveGame(int number) {}
    void loadGame(int number) {}
    void deleteGame(int number);

    int getSelectedSaveNumber() const;
    int getNewSaveNumber() const;
};

} // namespace game

} // namespace reone
