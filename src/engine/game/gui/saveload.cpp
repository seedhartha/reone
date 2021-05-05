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

#include "saveload.h"

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "../../common/log.h"
#include "../../common/streamutil.h"
#include "../../gui/control/listbox.h"
#include "../../resource/format/erfreader.h"
#include "../../resource/format/gffreader.h"
#include "../../resource/strings.h"

#include "../game.h"

#include "colorutil.h"

namespace fs = boost::filesystem;

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static const char kSavesDirectoryName[] = "saves";

static constexpr int kStrRefLoadGame = 1585;
static constexpr int kStrRefSave = 1587;
static constexpr int kStrRefSaveGame = 1588;
static constexpr int kStrRefLoad = 1589;

SaveLoad::SaveLoad(Game *game) :
    GameGUI(game->gameId(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("saveload");

    initForGame();

    if (_gameId == GameID::KotOR) {
        loadBackground(BackgroundType::Menu);
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
    protoItem.setBorderColorOverride(getBaseColor(_gameId));
    protoItem.setHilightColor(_defaultHilightColor);
}

void SaveLoad::refresh() {
    setControlDisabled("BTN_DELETE", _mode != Mode::Save);

    string panelName(Strings::instance().get(_mode == Mode::Save ? kStrRefSaveGame : kStrRefLoadGame));
    setControlText("LBL_PANELNAME", panelName);

    string actionName(Strings::instance().get(_mode == Mode::Save ? kStrRefSave : kStrRefLoad));
    setControlText("BTN_SAVELOAD", actionName);

    refreshSavedGames();
}

static fs::path getSavesPath() {
    fs::path savesPath(fs::current_path());
    savesPath.append(kSavesDirectoryName);
    return move(savesPath);
}

void SaveLoad::refreshSavedGames() {
    _saves.clear();

    fs::path savesPath(getSavesPath());
    if (!fs::exists(savesPath)) {
        fs::create_directory(savesPath);
    }
    for (auto &entry : fs::directory_iterator(savesPath)) {
        if (fs::is_regular_file(entry) && boost::to_lower_copy(entry.path().extension().string()) == ".sav") {
            indexSavedGame(entry);
        }
    }

    auto &lbGames = getControl<ListBox>("LB_GAMES");
    lbGames.clearItems();
    for (size_t i = 0; i < _saves.size(); ++i) {
        string name(str(boost::format("%06d") % _saves[i].number));
        ListBox::Item item;
        item.tag = name;
        item.text = name;
        lbGames.addItem(move(item));
    }
}

static SavedGame peekSavedGame(const fs::path &path) {
    ErfReader erf;
    erf.load(path);

    shared_ptr<ByteArray> nfoData(erf.find("savenfo", ResourceType::Res));

    GffReader nfo;
    nfo.load(wrap(nfoData));

    SavedGame result;
    result.lastModule = nfo.root()->getString("LastModule");

    return move(result);
}

void SaveLoad::indexSavedGame(fs::path path) {
    try {
        fs::path basename(path.filename());
        basename.replace_extension();
        int number = stoi(basename.string());

        SavedGameDescriptor descriptor;
        descriptor.number = number;
        descriptor.save = peekSavedGame(path);
        descriptor.path = move(path);
        _saves.push_back(move(descriptor));
    }
    catch (const exception &e) {
        warn("Error indexing a saved game: " + string(e.what()));
    }
}

void SaveLoad::setMode(Mode mode) {
    _mode = mode;
}

void SaveLoad::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_SAVELOAD") {
        int number = getSelectedSaveNumber();
        switch (_mode) {
            case Mode::Save:
                if (number == -1) {
                    number = getNewSaveNumber();
                }
                saveGame(number);
                refresh();
                break;
            default:
                if (number != -1) {
                    loadGame(number);
                }
                break;
        }
    } else if (control == "BTN_DELETE") {
        int number = getSelectedSaveNumber();
        if (number != -1) {
            deleteGame(number);
            refresh();
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

int SaveLoad::getSelectedSaveNumber() const {
    ListBox &lbGames = getControl<ListBox>("LB_GAMES");

    int hilightedIdx = lbGames.hilightedIndex();
    if (hilightedIdx == -1) return -1;

    string tag(lbGames.getItemAt(hilightedIdx).tag);

    return stoi(tag);
}

int SaveLoad::getNewSaveNumber() const {
    int number = 0;
    for (auto &save : _saves) {
        number = max(number, save.number);
    }
    return number + 1;
}

static fs::path getSaveGamePath(int number) {
    fs::path result(getSavesPath());
    result.append(str(boost::format("%06d") % number) + ".sav");
    return move(result);
}

void SaveLoad::saveGame(int number) {
    fs::path savPath(getSaveGamePath(number));
    _game->saveToFile(savPath);
    refresh();
}

void SaveLoad::loadGame(int number) {
    auto maybeSave = find_if(_saves.begin(), _saves.end(), [&number](auto &save) { return save.number == number; });
    if (maybeSave != _saves.end()) {
        _game->loadFromFile(maybeSave->path);
    }
}

void SaveLoad::deleteGame(int number) {
    auto maybeSave = find_if(_saves.begin(), _saves.end(), [&number](auto &save) { return save.number == number; });
    if (maybeSave != _saves.end()) {
        fs::remove(maybeSave->path);
        refresh();
    }
}

} // namespace game

} // namespace reone
