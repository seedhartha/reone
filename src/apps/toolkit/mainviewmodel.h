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
#include "reone/graphics/model.h"
#include "reone/resource/di/module.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/gff.h"
#include "reone/resource/id.h"
#include "reone/scene/di/module.h"
#include "reone/system/di/module.h"
#include "reone/system/stream/input.h"
#include "reone/tools/tool.h"
#include "reone/tools/types.h"

#include "eventhandler.h"

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
    resource::ResourceId resourceId;

    std::string textContent;

    Page(PageType type,
         std::string displayName,
         resource::ResourceId resourceId) :
        type(type),
        displayName(displayName),
        resourceId(resourceId) {
    }
};

struct PageRemovingEventData {
    int index {0};
    Page *page {nullptr};

    PageRemovingEventData() {}

    PageRemovingEventData(int index, Page *page) :
        index(index),
        page(page) {
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

    void playAnimation(const std::string &anim);

    void update3D();
    void render3D(int w, int h);

    game::GameID gameId() const { return _gameId; }
    const boost::filesystem::path &gamePath() const { return _gamePath; }

    int numGameDirItems() const { return static_cast<int>(_gameDirItems.size()); }
    GameDirectoryItem &gameDirItem(int index) { return *_gameDirItems[index]; }
    GameDirectoryItem &gameDirItemById(GameDirectoryItemId id) { return *_idToGameDirItem.at(id); }

    const Page &page(int index) {
        auto pageIterator = _pages.begin();
        std::advance(pageIterator, index);
        return *pageIterator;
    }

    EventHandler<Page *> &pageAdded() { return _pageAdded; }
    EventHandler<PageRemovingEventData> &pageRemoving() { return _pageRemoving; }
    EventHandler<int> &pageSelected() { return _pageSelected; }
    EventHandler<std::shared_ptr<TableContent>> &tableContent() { return _tableContent; }
    EventHandler<std::shared_ptr<TableContent>> &talkTableContent() { return _talkTableContent; }
    EventHandler<std::shared_ptr<resource::Gff>> &gffContent() { return _gffContent; }
    EventHandler<std::string> &nssContent() { return _nssContent; }
    EventHandler<std::string> &pcodeContent() { return _pcodeContent; }
    EventHandler<std::shared_ptr<ByteArray>> &imageData() { return _imageData; }
    EventHandler<std::string> &imageInfo() { return _imageInfo; }
    EventHandler<std::vector<std::string>> &animations() { return _animations; }
    EventHandler<std::shared_ptr<audio::AudioStream>> &audioStream() { return _audioStream; }
    EventHandler<Progress> &progress() { return _progress; }
    EventHandler<bool> &engineLoadRequested() { return _engineLoadRequested; }
    EventHandler<bool> &renderRequested() { return _renderRequested; }
    EventHandler<bool> &renderTimerEnabled() { return _renderTimerEnabled; }

    void onViewCreated();
    void onViewDestroyed();

    void onNotebookPageClose(int page);

    void onGameDirectoryChanged(boost::filesystem::path path);
    void onGameDirectoryItemIdentified(int index, GameDirectoryItemId id);
    void onGameDirectoryItemExpanding(GameDirectoryItemId id);
    void onGameDirectoryItemActivated(GameDirectoryItemId id);

    void onGLCanvasMouseMotion(int x, int y, bool leftDown, bool rightDown);
    void onGLCanvasMouseWheel(int delta);

private:
    boost::filesystem::path _gamePath;
    game::GameID _gameId {game::GameID::KotOR};

    std::vector<resource::KeyReader::KeyEntry> _keyKeys;
    std::vector<resource::KeyReader::FileEntry> _keyFiles;

    std::vector<std::shared_ptr<GameDirectoryItem>> _gameDirItems;
    std::map<GameDirectoryItemId, GameDirectoryItem *> _idToGameDirItem;

    std::vector<std::shared_ptr<Tool>> _tools;

    std::shared_ptr<scene::CameraSceneNode> _cameraNode;
    std::shared_ptr<graphics::Model> _model;
    std::shared_ptr<scene::ModelSceneNode> _modelNode;
    glm::vec3 _cameraPosition {0.0f};
    float _modelHeading {0.0f};
    float _modelPitch {0.0f};
    int _lastMouseX {0};
    int _lastMouseY {0};
    uint32_t _lastTicks {0};

    std::list<Page> _pages;

    // Event handlers

    EventHandler<Page *> _pageAdded;
    EventHandler<PageRemovingEventData> _pageRemoving;
    EventHandler<int> _pageSelected;
    EventHandler<std::shared_ptr<TableContent>> _tableContent;
    EventHandler<std::shared_ptr<TableContent>> _talkTableContent;
    EventHandler<std::shared_ptr<resource::Gff>> _gffContent;
    EventHandler<std::string> _nssContent;
    EventHandler<std::string> _pcodeContent;
    EventHandler<std::shared_ptr<ByteArray>> _imageData;
    EventHandler<std::string> _imageInfo;
    EventHandler<std::vector<std::string>> _animations;
    EventHandler<std::shared_ptr<audio::AudioStream>> _audioStream;
    EventHandler<Progress> _progress;
    EventHandler<bool> _engineLoadRequested;
    EventHandler<bool> _renderRequested;
    EventHandler<bool> _renderTimerEnabled;

    // END Event handlers

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

    void updateModelTransform();
    void updateCameraTransform();
};

} // namespace reone
