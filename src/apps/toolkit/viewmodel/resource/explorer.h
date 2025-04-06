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

#include "../../collectionproperty.h"
#include "../../property.h"
#include "../resource.h"

#include "audio.h"
#include "image.h"
#include "model.h"

namespace reone {

struct ResourcesItemId {
    std::filesystem::path path;
    std::shared_ptr<resource::ResourceId> resId;

    size_t hash() const {
        size_t hash = 0ll;
        boost::hash_combine(hash, path);
        if (resId) {
            boost::hash_combine(hash, resId->hash());
        }
        return hash;
    }

    inline bool operator==(const ResourcesItemId &rhs) const {
        return path == rhs.path && resId == rhs.resId;
    }

    inline bool operator!=(const ResourcesItemId &rhs) const {
        return path != rhs.path || resId != rhs.resId;
    }

    inline bool operator<(const ResourcesItemId &rhs) const {
        if (path < rhs.path) {
            return true;
        }
        if (path > rhs.path) {
            return false;
        }
        return resId < rhs.resId;
    }
};

struct ResourcesItem {
    ResourcesItemId id;
    std::optional<ResourcesItemId> parentId;
    std::string displayName;
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

    std::shared_ptr<ResourceViewModel> viewModel;

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

class ResourceExplorerViewModel : public ResourceViewModel {
public:
    ResourceExplorerViewModel();

    resource::GameID gameId() const {
        return _gameId;
    }

    const std::filesystem::path &gamePath() const {
        return _resourcesPath;
    }

    ResourcesItem &getResourcesItemById(const ResourcesItemId &id) {
        return *_idToResItem.at(id);
    }

    const resource::TalkTable &talkTable() const {
        return *_talkTable;
    }

    // View models

    ImageResourceViewModel &imageResViewModel() {
        return *_imageResViewModel;
    }

    ModelResourceViewModel &modelResViewModel() {
        return *_modelResViewModel;
    }

    AudioResourceViewModel &audioResViewModel() {
        return *_audioResViewModel;
    }

    // END View models

    // Properties

    CollectionProperty<ResourcesItem *> &resourcesItems() {
        return _resItems;
    }

    CollectionProperty<std::shared_ptr<Page>> &pages() {
        return _pages;
    }

    Property<int> &selectedPage() {
        return _selectedPage;
    }

    Property<Progress> &progress() {
        return _progress;
    }

    Property<bool> &renderEnabled() {
        return _renderEnabled;
    }

    Property<bool> &goToParentEnabled() {
        return _goToParentEnabled;
    }

    // END Properties

    // Commands

    void exportResource(ResourcesItemId itemId, const std::filesystem::path &destPath);
    void exportTgaTxi(ResourcesItemId itemId, const std::filesystem::path &destPath);
    void exportWavMp3(ResourcesItemId itemId, const std::filesystem::path &destPath);

    void createRim(ResourcesItemId itemId, const std::filesystem::path &destPath);
    void createErf(ResourcesItemId itemId, const std::filesystem::path &destPath);
    void createMod(ResourcesItemId itemId, const std::filesystem::path &destPath);

    void saveFile(Page &page, const std::filesystem::path &destPath);

    void extractArchive(const std::filesystem::path &srcPath, const std::filesystem::path &destPath);
    void decompile(ResourcesItemId itemId, bool optimize = true);

    void extractAllBifs(const std::filesystem::path &destPath);
    void batchConvertTpcToTga(const std::filesystem::path &srcPath, const std::filesystem::path &destPath);

    bool invokeTool(Operation operation,
                    const std::filesystem::path &srcPath,
                    const std::filesystem::path &destPath);

    // END Commands

    void onViewCreated();
    void onViewDestroyed();

    void onNotebookPageClose(int page);

    void onResourcesDirectoryChanged(resource::GameID gameId, std::filesystem::path path);
    void onResourcesListBoxDoubleClick(const ResourcesItemId &id);
    void onGoToParentButton();

private:
    resource::GameID _gameId {resource::GameID::KotOR};
    std::filesystem::path _resourcesPath;

    std::vector<resource::KeyReader::KeyEntry> _keyKeys;
    std::vector<resource::KeyReader::FileEntry> _keyFiles;

    std::shared_ptr<resource::TalkTable> _talkTable;
    std::unique_ptr<game::Routines> _routines;

    std::map<ResourcesItemId, ResourcesItem *> _idToResItem;
    std::list<std::shared_ptr<ResourcesItem>> _allResItems;
    std::optional<ResourcesItemId> _expandedItemId;

    std::vector<std::shared_ptr<Tool>> _tools;

    // View models

    std::unique_ptr<ImageResourceViewModel> _imageResViewModel;
    std::unique_ptr<ModelResourceViewModel> _modelResViewModel;
    std::unique_ptr<AudioResourceViewModel> _audioResViewModel;

    // END View models

    // Properties

    CollectionProperty<ResourcesItem *> _resItems;
    CollectionProperty<std::shared_ptr<Page>> _pages;

    Property<int> _selectedPage;
    Property<Progress> _progress;
    Property<bool> _renderEnabled;
    Property<bool> _goToParentEnabled;

    // END Properties

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

    void loadResources();
    void loadTools();
    void loadEngine();

    void openFile(const ResourcesItem &item);
    void openResource(const resource::ResourceId &id, IInputStream &data);

    PageType getPageType(resource::ResType type) const;

    void withResourceStream(const ResourcesItem &item, std::function<void(IInputStream &)> block);
};

} // namespace reone
