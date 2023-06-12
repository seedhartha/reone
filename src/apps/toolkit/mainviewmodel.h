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

#include "reone/game/types.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/id.h"
#include "reone/tools/tool.h"
#include "reone/tools/types.h"

namespace reone {

class MainViewModel : boost::noncopyable {
public:
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

    bool extractAllBifs(const boost::filesystem::path &destPath);
    bool batchConvertTpcToTga(const boost::filesystem::path &srcPath, const boost::filesystem::path &destPath);

    bool invokeTool(Operation operation,
                    const boost::filesystem::path &srcPath,
                    const boost::filesystem::path &destPath);

    game::GameID gameId() const { return _gameId; }
    const boost::filesystem::path &gamePath() const { return _gamePath; }

    int numGameDirItems() const { return static_cast<int>(_gameDirItems.size()); }
    GameDirectoryItem &gameDirItem(int index) { return *_gameDirItems[index]; }
    GameDirectoryItem &gameDirItemById(GameDirectoryItemId id) { return *_idToGameDirItem.at(id); }

    void onViewCreated();
    void onViewDestroyed();

    void onGameDirectoryChanged(boost::filesystem::path path);
    void onGameDirectoryItemIdentified(int index, GameDirectoryItemId id);
    void onGameDirectoryItemExpanding(GameDirectoryItemId id);

private:
    boost::filesystem::path _gamePath;
    game::GameID _gameId {game::GameID::KotOR};

    std::vector<resource::KeyReader::KeyEntry> _keyKeys;
    std::vector<resource::KeyReader::FileEntry> _keyFiles;

    std::vector<std::shared_ptr<GameDirectoryItem>> _gameDirItems;
    std::map<GameDirectoryItemId, GameDirectoryItem *> _idToGameDirItem;

    std::vector<std::shared_ptr<Tool>> _tools;

    void loadGameDirectory();
    void loadTools();
};

} // namespace reone
