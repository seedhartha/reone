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

#include "reone/game/game.h"
#include "reone/graphics/format/tgareader.h"
#include "reone/resource/format/erfreader.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/provider/erf.h"
#include "reone/resource/strings.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/memoryinput.h"

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

    _controls.LBL_PLANETNAME->setVisible(false);
    _controls.LBL_AREANAME->setVisible(false);

    _controls.LB_GAMES->setSelectionMode(ListBox::SelectionMode::OnClick);
    _controls.LB_GAMES->setPadding(3);
    _controls.LB_GAMES->protoItem().setUseBorderColorOverride(true);
    _controls.LB_GAMES->protoItem().setBorderColorOverride(_baseColor);
    _controls.LB_GAMES->protoItem().setHilightColor(_hilightColor);
    _controls.LB_GAMES->setOnItemClick([this](const std::string &item) {
        // Get save number by item tag
        int selectedSaveNumber = -1;
        for (int i = 0; i < _controls.LB_GAMES->getItemCount(); ++i) {
            auto &lbItem = _controls.LB_GAMES->getItemAt(i);
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
        _controls.LBL_SCREENSHOT->setBorderFill(std::move(screenshot));
    });

    _controls.BTN_SAVELOAD->setOnClick([this]() {
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
    _controls.BTN_DELETE->setOnClick([this]() {
        int number = getSelectedSaveNumber();
        if (number != -1) {
            deleteGame(number);
            refresh();
        }
    });
    _controls.BTN_BACK->setOnClick([this]() {
        _controls.LB_GAMES->clearSelection();
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

void SaveLoad::refresh() {
    _controls.BTN_DELETE->setDisabled(_mode != SaveLoadMode::Save);

    std::string panelName(_services.resource.strings.getText(_mode == SaveLoadMode::Save ? kStrRefSaveGame : kStrRefLoadGame));
    _controls.LBL_PANELNAME->setTextMessage(std::move(panelName));

    std::string actionName(_services.resource.strings.getText(_mode == SaveLoadMode::Save ? kStrRefSave : kStrRefLoad));
    _controls.BTN_SAVELOAD->setTextMessage(std::move(actionName));

    refreshSavedGames();
}

static std::filesystem::path getSavesPath() {
    std::filesystem::path savesPath(std::filesystem::current_path());
    savesPath.append(kSavesDirectoryName);
    return std::move(savesPath);
}

void SaveLoad::refreshSavedGames() {
    _saves.clear();

    std::filesystem::path savesPath(getSavesPath());
    if (!std::filesystem::exists(savesPath)) {
        std::filesystem::create_directory(savesPath);
    }
    for (auto &entry : std::filesystem::directory_iterator(savesPath)) {
        if (std::filesystem::is_regular_file(entry) && boost::to_lower_copy(entry.path().extension().string()) == ".sav") {
            indexSavedGame(entry);
        }
    }

    _controls.LB_GAMES->clearItems();
    for (size_t i = 0; i < _saves.size(); ++i) {
        std::string name(str(boost::format("%06d") % _saves[i].number));
        ListBox::Item item;
        item.tag = name;
        item.text = name;
        _controls.LB_GAMES->addItem(std::move(item));
    }
}

static SavedGame peekSavedGame(const std::filesystem::path &path) {
    auto erfResourceProvider = ErfResourceProvider(path);

    auto nfoData = erfResourceProvider.findResourceData(ResourceId("savenfo", ResourceType::Res));
    auto nfoStream = MemoryInputStream(*nfoData);
    GffReader nfo(nfoStream);
    nfo.load();

    std::shared_ptr<Texture> screen;
    auto screenData = erfResourceProvider.findResourceData(ResourceId("screen", ResourceType::Tga));
    if (screenData) {
        auto tga = MemoryInputStream(*screenData);
        TgaReader tgaReader(tga, "screen", TextureUsage::GUI);
        tgaReader.load();
        screen = tgaReader.texture();
    }

    SavedGame result;
    result.screen = std::move(screen);
    result.lastModule = nfo.root()->getString("LastModule");

    return std::move(result);
}

void SaveLoad::indexSavedGame(std::filesystem::path path) {
    try {
        std::filesystem::path basename(path.filename());
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
    int hilightedIdx = _controls.LB_GAMES->selectedItemIndex();
    if (hilightedIdx == -1)
        return -1;

    std::string tag(_controls.LB_GAMES->getItemAt(hilightedIdx).tag);

    return stoi(tag);
}

int SaveLoad::getNewSaveNumber() const {
    int number = 0;
    for (auto &save : _saves) {
        number = std::max(number, save.number);
    }
    return number + 1;
}

static std::filesystem::path getSaveGamePath(int number) {
    std::filesystem::path result(getSavesPath());
    result.append(str(boost::format("%06d") % number) + ".sav");
    return std::move(result);
}

void SaveLoad::deleteGame(int number) {
    auto maybeSave = std::find_if(_saves.begin(), _saves.end(), [&number](auto &save) { return save.number == number; });
    if (maybeSave != _saves.end()) {
        std::filesystem::remove(maybeSave->path);
        refresh();
    }
}

} // namespace game

} // namespace reone
