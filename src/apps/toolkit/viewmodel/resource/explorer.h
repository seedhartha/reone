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

#include "reone/audio/clip.h"
#include "reone/audio/di/module.h"
#include "reone/game/script/routines.h"
#include "reone/game/types.h"
#include "reone/graphics/di/module.h"
#include "reone/graphics/model.h"
#include "reone/resource/di/module.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/gff.h"
#include "reone/resource/id.h"
#include "reone/resource/talktable.h"
#include "reone/scene/di/module.h"
#include "reone/script/di/module.h"
#include "reone/system/di/module.h"
#include "reone/system/stream/input.h"
#include "reone/tools/legacy/tool.h"
#include "reone/tools/types.h"

#include "../../binding/collection.h"
#include "../../binding/property.h"
#include "../../viewmodel.h"

#include "audio.h"
#include "image.h"
#include "model.h"

namespace reone {

using GameDirectoryItemId = void *;

struct GameDirectoryItem {
    GameDirectoryItemId id {nullptr};
    GameDirectoryItemId parentId {nullptr};
    std::string displayName;
    std::filesystem::path path;
    std::shared_ptr<resource::ResourceId> resId;
    bool container {false};
    bool loaded {false};
    bool archived {false};
};

enum class PageType {
    Text,
    Table,
    GFF,
    NCS,
    NSS,
    Image,
    Model,
    Audio
};

struct Page : boost::noncopyable {
    PageType type;
    std::string displayName;
    resource::ResourceId resourceId;
    bool dirty {false};

    std::shared_ptr<ViewModel> viewModel;

    Page(PageType type,
         std::string displayName,
         resource::ResourceId resourceId) :
        type(type),
        displayName(displayName),
        resourceId(resourceId) {
    }
};

struct Progress {
    bool visible {false};
    std::string title;
    std::string message;
    int value {0};
};

class ResourceExplorerViewModel : public ViewModel {
public:
    ResourceExplorerViewModel() {
        _imageResViewModel = std::make_unique<ImageResourceViewModel>();
        _audioResViewModel = std::make_unique<AudioResourceViewModel>();
    }

    void extractArchive(const std::filesystem::path &srcPath, const std::filesystem::path &destPath);
    void decompile(GameDirectoryItemId itemId, bool optimize = true);
    void exportFile(GameDirectoryItemId itemId, const std::filesystem::path &destPath);

    void extractAllBifs(const std::filesystem::path &destPath);
    void batchConvertTpcToTga(const std::filesystem::path &srcPath, const std::filesystem::path &destPath);

    bool invokeTool(Operation operation,
                    const std::filesystem::path &srcPath,
                    const std::filesystem::path &destPath);

    resource::GameID gameId() const { return _gameId; }
    const std::filesystem::path &gamePath() const { return _gamePath; }

    int getGameDirItemCount() const { return static_cast<int>(_gameDirItems.size()); }
    GameDirectoryItem &getGameDirItem(int index) { return *_gameDirItems[index]; }
    GameDirectoryItem &getGameDirItemById(GameDirectoryItemId id) { return *_idToGameDirItem.at(id); }

    Page &getPage(int index) {
        return *_pages.at(index);
    }

    const resource::TalkTable &talkTable() const {
        return *_talkTable;
    }

    std::string getTalkTableText(int index) const { return _talkTable->getString(index).text; }
    std::string getTalkTableSound(int index) const { return _talkTable->getString(index).soundResRef; }

    Collection<std::shared_ptr<Page>> &pages() { return _pages; }
    Property<int> &selectedPage() { return _selectedPage; }
    Property<Progress> &progress() { return _progress; }
    Property<bool> &engineLoadRequested() { return _engineLoadRequested; }
    Property<bool> &renderEnabled() { return _renderEnabled; }

    ImageResourceViewModel &imageResViewModel() {
        return *_imageResViewModel;
    }

    ModelResourceViewModel &modelResViewModel() {
        return *_modelResViewModel;
    }

    AudioResourceViewModel &audioResViewModel() {
        return *_audioResViewModel;
    }

    void onViewCreated();
    void onViewDestroyed();

    void onNotebookPageClose(int page);

    void onGameDirectoryChanged(std::filesystem::path path);
    void onGameDirectoryItemIdentified(int index, GameDirectoryItemId id);
    void onGameDirectoryItemExpanding(GameDirectoryItemId id);
    void onGameDirectoryItemActivated(GameDirectoryItemId id);

private:
    std::filesystem::path _gamePath;
    resource::GameID _gameId {resource::GameID::KotOR};

    std::vector<resource::KeyReader::KeyEntry> _keyKeys;
    std::vector<resource::KeyReader::FileEntry> _keyFiles;

    std::shared_ptr<resource::TalkTable> _talkTable;
    std::unique_ptr<game::Routines> _routines;

    std::vector<std::shared_ptr<GameDirectoryItem>> _gameDirItems;
    std::map<GameDirectoryItemId, GameDirectoryItem *> _idToGameDirItem;

    std::vector<std::shared_ptr<Tool>> _tools;

    std::unique_ptr<ImageResourceViewModel> _imageResViewModel;
    std::unique_ptr<ModelResourceViewModel> _modelResViewModel;
    std::unique_ptr<AudioResourceViewModel> _audioResViewModel;

    // Event handlers

    Collection<std::shared_ptr<Page>> _pages;
    Property<int> _selectedPage;
    Property<Progress> _progress;
    Property<bool> _engineLoadRequested;
    Property<bool> _renderEnabled;

    // END Event handlers

    // Embedded engine

    graphics::GraphicsOptions _graphicsOpt;
    audio::AudioOptions _audioOpt;

    std::unique_ptr<IClock> _clock;
    std::unique_ptr<SystemModule> _systemModule;
    std::unique_ptr<resource::ResourceModule> _resourceModule;
    std::unique_ptr<graphics::GraphicsModule> _graphicsModule;
    std::unique_ptr<audio::AudioModule> _audioModule;
    std::unique_ptr<scene::SceneModule> _sceneModule;
    std::unique_ptr<script::ScriptModule> _scriptModule;

    bool _engineLoaded {false};

    // END Embedded engine

    void loadGameDirectory();
    void loadTools();
    void loadEngine();

    void openFile(const GameDirectoryItem &item);
    void openResource(const resource::ResourceId &id, IInputStream &data);

    PageType getPageType(resource::ResType type) const;

    void withResourceStream(const GameDirectoryItem &item, std::function<void(IInputStream &)> block);
};

} // namespace reone
