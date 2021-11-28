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

#include "../../common/logutil.h"
#include "../../common/streamutil.h"
#include "../../graphics/format/tgareader.h"
#include "../../resource/format/erfreader.h"
#include "../../resource/format/gffreader.h"
#include "../../resource/strings.h"

#include "../kotor.h"

namespace fs = boost::filesystem;

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace kotor {

static const char kSavesDirectoryName[] = "saves";

static constexpr int kStrRefLoadGame = 1585;
static constexpr int kStrRefSave = 1587;
static constexpr int kStrRefSaveGame = 1588;
static constexpr int kStrRefLoad = 1589;

SaveLoad::SaveLoad(KotOR &game, Services &services) :
    GameGUI(game, services) {
    _resRef = getResRef("saveload");

    initForGame();

    if (!game.isTSL()) {
        loadBackground(BackgroundType::Menu);
    }
}

void SaveLoad::load() {
    GUI::load();
    bindControls();

    _binding.lblPlanetName->setVisible(false);
    _binding.lblAreaName->setVisible(false);

    _binding.lbGames->setSelectionMode(ListBox::SelectionMode::OnClick);
    _binding.lbGames->setPadding(3);
    _binding.lbGames->protoItem().setUseBorderColorOverride(true);
    _binding.lbGames->protoItem().setBorderColorOverride(_game.getGUIColorBase());
    _binding.lbGames->protoItem().setHilightColor(_defaultHilightColor);
    _binding.lbGames->setOnItemClick([this](const string &item) {
        // Get save number by item tag
        int selectedSaveNumber = -1;
        for (int i = 0; i < _binding.lbGames->getItemCount(); ++i) {
            auto &lbItem = _binding.lbGames->getItemAt(i);
            if (lbItem.tag == item) {
                selectedSaveNumber = stoi(lbItem.tag);
                break;
            }
        }

        // Get save screenshot by save number
        shared_ptr<Texture> screenshot;
        if (selectedSaveNumber != -1) {
            for (auto &save : _saves) {
                if (save.number == selectedSaveNumber) {
                    screenshot = save.save.screen;
                    break;
                }
            }
        }

        // Set screenshot
        _binding.lblScreenshot->setBorderFill(move(screenshot));
    });

    _binding.btnSaveLoad->setOnClick([this]() {
        int number = getSelectedSaveNumber();
        switch (_mode) {
        case SaveLoadMode::Save:
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
    });
    _binding.btnDelete->setOnClick([this]() {
        int number = getSelectedSaveNumber();
        if (number != -1) {
            deleteGame(number);
            refresh();
        }
    });
    _binding.btnBack->setOnClick([this]() {
        _binding.lbGames->clearSelection();
        switch (_mode) {
        case SaveLoadMode::Save:
        case SaveLoadMode::LoadFromInGame:
            _game.openInGame();
            break;
        default:
            _game.openMainMenu();
            break;
        }
    });
}

void SaveLoad::bindControls() {
    _binding.btnBack = getControl<Button>("BTN_BACK");
    _binding.btnDelete = getControl<Button>("BTN_DELETE");
    _binding.btnSaveLoad = getControl<Button>("BTN_SAVELOAD");
    _binding.lblAreaName = getControl<Label>("LBL_AREANAME");
    _binding.lblPanelName = getControl<Label>("LBL_PANELNAME");
    _binding.lblPlanetName = getControl<Label>("LBL_PLANETNAME");
    _binding.lblPm1 = getControl<Label>("LBL_PM1");
    _binding.lblPm2 = getControl<Label>("LBL_PM2");
    _binding.lblPm3 = getControl<Label>("LBL_PM3");
    _binding.lblScreenshot = getControl<Label>("LBL_SCREENSHOT");
    _binding.lbGames = getControl<ListBox>("LB_GAMES");

    if (_game.isTSL()) {
        _binding.btnFilter = getControl<Button>("BTN_FILTER");
        _binding.lblBar1 = getControl<Label>("LBL_BAR1");
        _binding.lblBar2 = getControl<Label>("LBL_BAR2");
        _binding.lblBar3 = getControl<Label>("LBL_BAR3");
        _binding.lblBar4 = getControl<Label>("LBL_BAR4");
        _binding.lblPcName = getControl<Label>("LBL_PCNAME");
        _binding.lblTimePlayed = getControl<Label>("LBL_TIMEPLAYED");
    }
}

void SaveLoad::refresh() {
    _binding.btnDelete->setDisabled(_mode != SaveLoadMode::Save);

    string panelName(_strings.get(_mode == SaveLoadMode::Save ? kStrRefSaveGame : kStrRefLoadGame));
    _binding.lblPanelName->setTextMessage(move(panelName));

    string actionName(_strings.get(_mode == SaveLoadMode::Save ? kStrRefSave : kStrRefLoad));
    _binding.btnSaveLoad->setTextMessage(move(actionName));

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

    _binding.lbGames->clearItems();
    for (size_t i = 0; i < _saves.size(); ++i) {
        string name(str(boost::format("%06d") % _saves[i].number));
        ListBox::Item item;
        item.tag = name;
        item.text = name;
        _binding.lbGames->addItem(move(item));
    }
}

static SavedGame peekSavedGame(const fs::path &path) {
    ErfReader erf;
    erf.load(path);

    shared_ptr<ByteArray> nfoData(erf.find(ResourceId("savenfo", ResourceType::Res)));
    GffReader nfo;
    nfo.load(wrap(nfoData));

    shared_ptr<Texture> screen;
    shared_ptr<ByteArray> screenData(erf.find(ResourceId("screen", ResourceType::Tga)));
    if (screenData) {
        TgaReader tga("screen", TextureUsage::GUI);
        tga.load(wrap(screenData));
        screen = tga.texture();
    }

    SavedGame result;
    result.screen = move(screen);
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
    } catch (const exception &e) {
        warn("Error indexing a saved game: " + string(e.what()));
    }
}

void SaveLoad::setMode(SaveLoadMode mode) {
    _mode = mode;
}

int SaveLoad::getSelectedSaveNumber() const {
    int hilightedIdx = _binding.lbGames->selectedItemIndex();
    if (hilightedIdx == -1)
        return -1;

    string tag(_binding.lbGames->getItemAt(hilightedIdx).tag);

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
    _game.saveToFile(savPath);
    refresh();
}

void SaveLoad::loadGame(int number) {
    auto maybeSave = find_if(_saves.begin(), _saves.end(), [&number](auto &save) { return save.number == number; });
    if (maybeSave != _saves.end()) {
        _game.loadFromFile(maybeSave->path);
    }
}

void SaveLoad::deleteGame(int number) {
    auto maybeSave = find_if(_saves.begin(), _saves.end(), [&number](auto &save) { return save.number == number; });
    if (maybeSave != _saves.end()) {
        fs::remove(maybeSave->path);
        refresh();
    }
}

} // namespace kotor

} // namespace reone
