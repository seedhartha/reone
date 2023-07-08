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

#include "reone/game/gui/saveload.h"

#include "reone/graphics/format/tgareader.h"
#include "reone/resource/format/erfreader.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/provider/erf.h"
#include "reone/resource/strings.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/bytearrayinput.h"

#include "reone/game/game.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static const char kSavesDirectoryName[] = "saves";

static constexpr int kStrRefLoadGame = 1585;
static constexpr int kStrRefSave = 1587;
static constexpr int kStrRefSaveGame = 1588;
static constexpr int kStrRefLoad = 1589;

void SaveLoad::onGUILoaded() {
    if (!_game.isTSL()) {
        loadBackground(BackgroundType::Menu);
    }

    bindControls();

    _binding.lblPlanetName->setVisible(false);
    _binding.lblAreaName->setVisible(false);

    _binding.lbGames->setSelectionMode(ListBox::SelectionMode::OnClick);
    _binding.lbGames->setPadding(3);
    _binding.lbGames->protoItem().setUseBorderColorOverride(true);
    _binding.lbGames->protoItem().setBorderColorOverride(_baseColor);
    _binding.lbGames->protoItem().setHilightColor(_hilightColor);
    _binding.lbGames->setOnItemClick([this](const std::string &item) {
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
        std::shared_ptr<Texture> screenshot;
        if (selectedSaveNumber != -1) {
            for (auto &save : _saves) {
                if (save.number == selectedSaveNumber) {
                    screenshot = save.save.screen;
                    break;
                }
            }
        }

        // Set screenshot
        _binding.lblScreenshot->setBorderFill(std::move(screenshot));
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
    _binding.btnBack = findControl<Button>("BTN_BACK");
    _binding.btnDelete = findControl<Button>("BTN_DELETE");
    _binding.btnSaveLoad = findControl<Button>("BTN_SAVELOAD");
    _binding.lblAreaName = findControl<Label>("LBL_AREANAME");
    _binding.lblPanelName = findControl<Label>("LBL_PANELNAME");
    _binding.lblPlanetName = findControl<Label>("LBL_PLANETNAME");
    _binding.lblPm1 = findControl<Label>("LBL_PM1");
    _binding.lblPm2 = findControl<Label>("LBL_PM2");
    _binding.lblPm3 = findControl<Label>("LBL_PM3");
    _binding.lblScreenshot = findControl<Label>("LBL_SCREENSHOT");
    _binding.lbGames = findControl<ListBox>("LB_GAMES");

    if (_game.isTSL()) {
        _binding.btnFilter = findControl<Button>("BTN_FILTER");
        _binding.lblBar1 = findControl<Label>("LBL_BAR1");
        _binding.lblBar2 = findControl<Label>("LBL_BAR2");
        _binding.lblBar3 = findControl<Label>("LBL_BAR3");
        _binding.lblBar4 = findControl<Label>("LBL_BAR4");
        _binding.lblPcName = findControl<Label>("LBL_PCNAME");
        _binding.lblTimePlayed = findControl<Label>("LBL_TIMEPLAYED");
    }
}

void SaveLoad::refresh() {
    _binding.btnDelete->setDisabled(_mode != SaveLoadMode::Save);

    std::string panelName(_services.resource.strings.get(_mode == SaveLoadMode::Save ? kStrRefSaveGame : kStrRefLoadGame));
    _binding.lblPanelName->setTextMessage(std::move(panelName));

    std::string actionName(_services.resource.strings.get(_mode == SaveLoadMode::Save ? kStrRefSave : kStrRefLoad));
    _binding.btnSaveLoad->setTextMessage(std::move(actionName));

    refreshSavedGames();
}

static boost::filesystem::path getSavesPath() {
    boost::filesystem::path savesPath(boost::filesystem::current_path());
    savesPath.append(kSavesDirectoryName);
    return std::move(savesPath);
}

void SaveLoad::refreshSavedGames() {
    _saves.clear();

    boost::filesystem::path savesPath(getSavesPath());
    if (!boost::filesystem::exists(savesPath)) {
        boost::filesystem::create_directory(savesPath);
    }
    for (auto &entry : boost::filesystem::directory_iterator(savesPath)) {
        if (boost::filesystem::is_regular_file(entry) && boost::to_lower_copy(entry.path().extension().string()) == ".sav") {
            indexSavedGame(entry);
        }
    }

    _binding.lbGames->clearItems();
    for (size_t i = 0; i < _saves.size(); ++i) {
        std::string name(str(boost::format("%06d") % _saves[i].number));
        ListBox::Item item;
        item.tag = name;
        item.text = name;
        _binding.lbGames->addItem(std::move(item));
    }
}

static SavedGame peekSavedGame(const boost::filesystem::path &path) {
    auto erfResourceProvider = ErfResourceProvider(path);

    auto nfoData = erfResourceProvider.find(ResourceId("savenfo", ResourceType::Res));
    auto nfoStream = ByteArrayInputStream(*nfoData);
    GffReader nfo;
    nfo.load(nfoStream);

    std::shared_ptr<Texture> screen;
    auto screenData = erfResourceProvider.find(ResourceId("screen", ResourceType::Tga));
    if (screenData) {
        auto tgaStream = ByteArrayInputStream(*screenData);
        TgaReader tga("screen", TextureUsage::GUI);
        tga.load(tgaStream);
        screen = tga.texture();
    }

    SavedGame result;
    result.screen = std::move(screen);
    result.lastModule = nfo.root()->getString("LastModule");

    return std::move(result);
}

void SaveLoad::indexSavedGame(boost::filesystem::path path) {
    try {
        boost::filesystem::path basename(path.filename());
        basename.replace_extension();
        int number = stoi(basename.string());

        SavedGameDescriptor descriptor;
        descriptor.number = number;
        descriptor.save = peekSavedGame(path);
        descriptor.path = std::move(path);
        _saves.push_back(std::move(descriptor));
    } catch (const std::exception &e) {
        warn("Error indexing a saved game: " + std::string(e.what()));
    }
}

void SaveLoad::setMode(SaveLoadMode mode) {
    _mode = mode;
}

int SaveLoad::getSelectedSaveNumber() const {
    int hilightedIdx = _binding.lbGames->selectedItemIndex();
    if (hilightedIdx == -1)
        return -1;

    std::string tag(_binding.lbGames->getItemAt(hilightedIdx).tag);

    return stoi(tag);
}

int SaveLoad::getNewSaveNumber() const {
    int number = 0;
    for (auto &save : _saves) {
        number = std::max(number, save.number);
    }
    return number + 1;
}

static boost::filesystem::path getSaveGamePath(int number) {
    boost::filesystem::path result(getSavesPath());
    result.append(str(boost::format("%06d") % number) + ".sav");
    return std::move(result);
}

void SaveLoad::deleteGame(int number) {
    auto maybeSave = std::find_if(_saves.begin(), _saves.end(), [&number](auto &save) { return save.number == number; });
    if (maybeSave != _saves.end()) {
        boost::filesystem::remove(maybeSave->path);
        refresh();
    }
}

} // namespace game

} // namespace reone
