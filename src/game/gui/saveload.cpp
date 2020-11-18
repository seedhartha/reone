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

#include "saveload.h"

#include <boost/filesystem.hpp>

#include "../../common/log.h"
#include "../../gui/control/listbox.h"
#include "../../resource/resources.h"

#include "../game.h"
#include "../savedgame.h"

#include "colors.h"

namespace fs = boost::filesystem;

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static const char kSavesDirectoryName[] = "saves";
static const char kSaveFilename[] = "savegame.sav";

static const int kStrRefLoadGame = 1585;
static const int kStrRefSave = 1587;
static const int kStrRefSaveGame = 1588;
static const int kStrRefLoad = 1589;

SaveLoad::SaveLoad(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("saveload");
    _backgroundType = BackgroundType::Menu;

    if (game->version() == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
}

void SaveLoad::load() {
    GUI::load();

    hideControl("LBL_PLANETNAME");
    hideControl("LBL_AREANAME");

    ListBox &lbGames = getControl<ListBox>("LB_GAMES");
    lbGames.setSelectionMode(ListBox::SelectionMode::Hilight);
    lbGames.setPadding(3);

    Control &protoItem = lbGames.protoItem();
    protoItem.setUseBorderColorOverride(true);
    protoItem.setBorderColorOverride(getBaseColor(_version));
    protoItem.setHilightColor(_defaultHilightColor);
}

void SaveLoad::update() {
    string panelName(Resources::instance().getString(_mode == Mode::Save ? kStrRefSaveGame : kStrRefLoadGame));

    Control &lblPanelName = getControl("LBL_PANELNAME");
    lblPanelName.setTextMessage(panelName);

    string actionName(Resources::instance().getString(_mode == Mode::Save ? kStrRefSave : kStrRefLoad));

    setControlDisabled("BTN_DELETE", _mode != Mode::Save);

    Control &btnSaveLoad = getControl("BTN_SAVELOAD");
    btnSaveLoad.setTextMessage(actionName);

    indexSavedGames();

    ListBox &lbGames = getControl<ListBox>("LB_GAMES");
    lbGames.clear();

    for (auto &save : _saves) {
        ListBox::Item item;
        item.tag = to_string(save.index);
        item.text = save.name;
        lbGames.add(move(item));
    }
}

void SaveLoad::indexSavedGames() {
    _saves.clear();

    fs::path savesPath(getSavesPath());
    if (!fs::exists(savesPath)) {
        fs::create_directory(savesPath);
    }
    for (auto &entry : fs::directory_iterator(savesPath)) {
        if (!fs::is_directory(entry)) continue;

        int saveIdx = stoi(entry.path().filename().string());
        indexSavedGame(saveIdx, entry.path());
    }
}

fs::path SaveLoad::getSavesPath() const {
    fs::path savesPath(fs::current_path());
    savesPath.append(kSavesDirectoryName);
    return move(savesPath);
}

void SaveLoad::indexSavedGame(int index, const fs::path &path) {
    fs::path savPath(path);
    savPath.append(kSaveFilename);

    if (!fs::exists(savPath)) {
        warn("SaveLoad: SAV file not found");
        return;
    }
    SavedGame sav(savPath);
    sav.peek();

    GameDescriptor save;
    save.index = index;
    save.path = savPath;
    save.name = sav.name();
    _saves.push_back(move(save));
}

void SaveLoad::setMode(Mode mode) {
    _mode = mode;
}

void SaveLoad::onClick(const string &control) {
    if (control == "BTN_SAVELOAD") {
        int saveIdx = getSelectedSaveIndex();
        switch (_mode) {
            case Mode::Save:
                if (saveIdx == -1) {
                    saveIdx = getNewSaveIndex();
                }
                saveGame(saveIdx);
                update();
                break;
            default:
                if (saveIdx != -1) {
                    loadGame(saveIdx);
                }
                break;
        }
    } else if (control == "BTN_DELETE") {
        int saveIdx = getSelectedSaveIndex();
        if (saveIdx != -1) {
            deleteGame(saveIdx);
            update();
        }
    } else if (control == "BTN_BACK") {
        ListBox &lbGames = getControl<ListBox>("LB_GAMES");
        lbGames.clearSelection();

        switch (_mode) {
            case Mode::Save:
            case Mode::LoadFromInGame:
                _game->openInGame();
                break;
            default:
                _game->openMainMenu();
                break;
        }
    }
}

int SaveLoad::getSelectedSaveIndex() const {
    ListBox &lbGames = getControl<ListBox>("LB_GAMES");

    int hilightedIdx = lbGames.hilightedIndex();
    if (hilightedIdx == -1) return -1;

    string tag(lbGames.getItemAt(hilightedIdx).tag);

    return stoi(tag);
}

int SaveLoad::getNewSaveIndex() const {
    fs::path savesPath(getSavesPath());
    int saveIdx = 1;
    for (auto &entry : fs::directory_iterator(savesPath)) {
        int idx = stoi(entry.path().filename().string()) + 1;
        if (saveIdx < idx) {
            saveIdx = idx;
        }
    }
    return saveIdx;
}

void SaveLoad::saveGame(int index) {
    fs::path saveDirPath(getSaveDirPath(index));
    fs::create_directory(saveDirPath);

    fs::path savPath(saveDirPath);
    savPath.append(kSaveFilename);

    SavedGame sav(savPath);
    sav.save(_game, getSaveName(index));
}

fs::path SaveLoad::getSaveDirPath(int index) const {
    fs::path saveDirPath(getSavesPath());
    saveDirPath.append(getSaveName(index));
    return move(saveDirPath);
}

string SaveLoad::getSaveName(int index) const {
    return str(boost::format("%06d") % index);
}

void SaveLoad::loadGame(int index) {
    auto maybeSave = find_if(_saves.begin(), _saves.end(), [&index](const GameDescriptor &save) { return save.index == index; });
    if (maybeSave == _saves.end()) return;

    SavedGame sav(maybeSave->path);
    sav.load(_game);
}

void SaveLoad::deleteGame(int index) {
    auto maybeSave = find_if(_saves.begin(), _saves.end(), [&index](const GameDescriptor &save) { return save.index == index; });
    if (maybeSave == _saves.end()) return;

    fs::path saveDirPath(getSaveDirPath(index));
    fs::remove_all(saveDirPath);
}

} // namespace game

} // namespace reone
