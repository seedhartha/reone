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

#include "reone/audio/buffer.h"
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
#include "reone/system/di/module.h"
#include "reone/system/stream/input.h"
#include "reone/tools/tool.h"
#include "reone/tools/types.h"

#include "eventhandler.h"

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

struct TableContent {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;

    TableContent(std::vector<std::string> columns, std::vector<std::vector<std::string>> rows) :
        columns(columns),
        rows(rows) {
    }
};

struct Page : boost::noncopyable {
    PageType type;
    std::string displayName;
    resource::ResourceId resourceId;

    std::string textContent;
    std::string xmlContent;
    std::shared_ptr<TableContent> tableContent;
    std::shared_ptr<resource::Gff> gffContent;
    std::string pcodeContent;
    std::string nssContent;
    bool dirty {false};

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

struct ImageContent {
    std::shared_ptr<ByteBuffer> tgaBytes;
    std::shared_ptr<ByteBuffer> txiBytes;

    ImageContent() {
    }

    ImageContent(std::shared_ptr<ByteBuffer> tgaBytes, std::shared_ptr<ByteBuffer> txiBytes) :
        tgaBytes(tgaBytes),
        txiBytes(txiBytes) {
    }
};

struct Progress {
    bool visible {false};
    std::string title;
    std::string message;
    int value {0};
};

class MainViewModel : boost::noncopyable {
public:
    void extractArchive(const std::filesystem::path &srcPath, const std::filesystem::path &destPath);
    void decompile(GameDirectoryItemId itemId, bool optimize = true);
    void exportFile(GameDirectoryItemId itemId, const std::filesystem::path &destPath);

    void extractAllBifs(const std::filesystem::path &destPath);
    void batchConvertTpcToTga(const std::filesystem::path &srcPath, const std::filesystem::path &destPath);

    bool invokeTool(Operation operation,
                    const std::filesystem::path &srcPath,
                    const std::filesystem::path &destPath);

    void playAnimation(const std::string &anim);

    void update3D();
    void render3D(int w, int h);

    game::GameID gameId() const { return _gameId; }
    const std::filesystem::path &gamePath() const { return _gamePath; }

    int getGameDirItemCount() const { return static_cast<int>(_gameDirItems.size()); }
    GameDirectoryItem &getGameDirItem(int index) { return *_gameDirItems[index]; }
    GameDirectoryItem &getGameDirItemById(GameDirectoryItemId id) { return *_idToGameDirItem.at(id); }

    Page &getPage(int index) {
        auto pageIterator = _pages.begin();
        std::advance(pageIterator, index);
        return **pageIterator;
    }

    std::string getTalkTableText(int index) const { return _talkTable->getString(index).text; }
    std::string getTalkTableSound(int index) const { return _talkTable->getString(index).soundResRef; }

    bool isRenderEnabled() const { return _renderEnabled; }

    EventHandler<Page *> &pageAdded() { return _pageAdded; }
    EventHandler<PageRemovingEventData> &pageRemoving() { return _pageRemoving; }
    EventHandler<int> &pageSelected() { return _pageSelected; }
    EventHandler<ImageContent> &imageChanged() { return _imageChanged; }
    EventHandler<std::vector<std::string>> &animations() { return _animations; }
    EventHandler<std::shared_ptr<audio::AudioBuffer>> &audioStream() { return _audioStream; }
    EventHandler<Progress> &progress() { return _progress; }
    EventHandler<bool> &engineLoadRequested() { return _engineLoadRequested; }

    void onViewCreated();
    void onViewDestroyed();

    void onNotebookPageClose(int page);

    void onGameDirectoryChanged(std::filesystem::path path);
    void onGameDirectoryItemIdentified(int index, GameDirectoryItemId id);
    void onGameDirectoryItemExpanding(GameDirectoryItemId id);
    void onGameDirectoryItemActivated(GameDirectoryItemId id);

    void onGLCanvasMouseMotion(int x, int y, bool leftDown, bool rightDown);
    void onGLCanvasMouseWheel(int delta);

private:
    std::filesystem::path _gamePath;
    game::GameID _gameId {game::GameID::KotOR};

    std::vector<resource::KeyReader::KeyEntry> _keyKeys;
    std::vector<resource::KeyReader::FileEntry> _keyFiles;

    std::shared_ptr<resource::TalkTable> _talkTable;
    std::unique_ptr<game::Routines> _routines;

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

    std::list<std::shared_ptr<Page>> _pages;

    // Event handlers

    EventHandler<Page *> _pageAdded;
    EventHandler<PageRemovingEventData> _pageRemoving;
    EventHandler<int> _pageSelected;
    EventHandler<ImageContent> _imageChanged;
    EventHandler<std::vector<std::string>> _animations;
    EventHandler<std::shared_ptr<audio::AudioBuffer>> _audioStream;
    EventHandler<Progress> _progress;
    EventHandler<bool> _engineLoadRequested;

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
    bool _renderEnabled {false};

    // END Embedded engine

    void loadGameDirectory();
    void loadTools();
    void loadEngine();

    void openFile(const GameDirectoryItem &item);
    void openResource(const resource::ResourceId &id, IInputStream &data);

    void updateModelTransform();
    void updateCameraTransform();

    PageType getPageType(resource::ResourceType type) const;

    void withResourceStream(const GameDirectoryItem &item, std::function<void(IInputStream &)> block);
};

} // namespace reone
