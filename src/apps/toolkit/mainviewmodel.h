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

#include "reone/audio/di/module.h"
#include "reone/audio/stream.h"
#include "reone/game/types.h"
#include "reone/graphics/di/module.h"
#include "reone/resource/di/module.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/gff.h"
#include "reone/resource/id.h"
#include "reone/scene/di/module.h"
#include "reone/system/di/module.h"
#include "reone/system/stream/input.h"
#include "reone/tools/tool.h"
#include "reone/tools/types.h"

#include "livedata.h"

namespace reone {

typedef void *GameDirectoryItemId;

struct GameDirectoryItem {
    GameDirectoryItemId id {nullptr};
    GameDirectoryItemId parentId {nullptr};
    std::string displayName;
    boost::filesystem::path path;
    std::shared_ptr<resource::ResourceId> resId;
    bool container {false};
    bool loaded {false};
    bool archived {false};
};

enum class PageType {
    Text,
    XML,
    Table,
    TalkTable,
    GFF,
    NSS,
    PCODE,
    Image,
    Model,
    Audio
};

struct Page {
    PageType type;
    std::string displayName;

    Page(PageType type, std::string displayName) :
        type(type),
        displayName(displayName) {
    }
};

struct Progress {
    bool visible {false};
    std::string title;
    std::string message;
    int value {0};
};

struct TableContent {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;

    TableContent(std::vector<std::string> columns, std::vector<std::vector<std::string>> rows) :
        columns(columns),
        rows(rows) {
    }
};

class MainViewModel : boost::noncopyable {
public:
    void openFile(const GameDirectoryItem &item);
    void openResource(const resource::ResourceId &id, IInputStream &data);

    void extractArchive(const boost::filesystem::path &srcPath, const boost::filesystem::path &destPath);
    void extractAllBifs(const boost::filesystem::path &destPath);
    void batchConvertTpcToTga(const boost::filesystem::path &srcPath, const boost::filesystem::path &destPath);

    bool invokeTool(Operation operation,
                    const boost::filesystem::path &srcPath,
                    const boost::filesystem::path &destPath);

    void render3D(int w, int h);

    game::GameID gameId() const { return _gameId; }
    const boost::filesystem::path &gamePath() const { return _gamePath; }

    int numGameDirItems() const { return static_cast<int>(_gameDirItems.size()); }
    GameDirectoryItem &gameDirItem(int index) { return *_gameDirItems[index]; }
    GameDirectoryItem &gameDirItemById(GameDirectoryItemId id) { return *_idToGameDirItem.at(id); }

    LiveData<std::list<Page>> &pages() { return _pages; }
    LiveData<std::string> &textContent() { return _textContent; }
    LiveData<std::shared_ptr<TableContent>> &tableContent() { return _tableContent; }
    LiveData<std::shared_ptr<TableContent>> &talkTableContent() { return _talkTableContent; }
    LiveData<std::shared_ptr<resource::Gff>> &gffContent() { return _gffContent; }
    LiveData<std::string> &nssContent() { return _nssContent; }
    LiveData<std::string> &pcodeContent() { return _pcodeContent; }
    LiveData<std::shared_ptr<ByteArray>> &imageData() { return _imageData; }
    LiveData<std::string> &imageInfo() { return _imageInfo; }
    LiveData<std::shared_ptr<audio::AudioStream>> &audioStream() { return _audioStream; }
    LiveData<Progress> &progress() { return _progress; }
    LiveData<bool> &engineLoadRequested() { return _engineLoadRequested; }

    void onViewCreated();
    void onViewDestroyed();

    void onGameDirectoryChanged(boost::filesystem::path path);
    void onGameDirectoryItemIdentified(int index, GameDirectoryItemId id);
    void onGameDirectoryItemExpanding(GameDirectoryItemId id);
    void onGameDirectoryItemActivated(GameDirectoryItemId id);

private:
    boost::filesystem::path _gamePath;
    game::GameID _gameId {game::GameID::KotOR};

    std::vector<resource::KeyReader::KeyEntry> _keyKeys;
    std::vector<resource::KeyReader::FileEntry> _keyFiles;

    std::vector<std::shared_ptr<GameDirectoryItem>> _gameDirItems;
    std::map<GameDirectoryItemId, GameDirectoryItem *> _idToGameDirItem;

    std::vector<std::shared_ptr<Tool>> _tools;

    // Live data

    LiveData<std::list<Page>> _pages;
    LiveData<std::shared_ptr<TableContent>> _tableContent;
    LiveData<std::shared_ptr<TableContent>> _talkTableContent;
    LiveData<std::shared_ptr<resource::Gff>> _gffContent;
    LiveData<std::string> _textContent;
    LiveData<std::string> _nssContent;
    LiveData<std::string> _pcodeContent;
    LiveData<std::shared_ptr<ByteArray>> _imageData;
    LiveData<std::string> _imageInfo;
    LiveData<std::shared_ptr<audio::AudioStream>> _audioStream;
    LiveData<Progress> _progress;
    LiveData<bool> _engineLoadRequested;

    // END Live data

    // Embedded engine

    graphics::GraphicsOptions _graphicsOpt;
    audio::AudioOptions _audioOpt;

    std::unique_ptr<SystemModule> _systemModule;
    std::unique_ptr<resource::ResourceModule> _resourceModule;
    std::unique_ptr<graphics::GraphicsModule> _graphicsModule;
    std::unique_ptr<audio::AudioModule> _audioModule;
    std::unique_ptr<scene::SceneModule> _sceneModule;

    bool _engineLoaded {false};

    // END Embedded engine

    void loadGameDirectory();
    void loadTools();
    void loadEngine();
};

} // namespace reone
