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
#include "reone/tools/tool.h"
#include "reone/tools/types.h"

namespace reone {

class MainViewModel : boost::noncopyable {
public:
    bool invokeTool(Operation operation,
                    const boost::filesystem::path &srcPath,
                    const boost::filesystem::path &destPath);

    void onViewCreated();
    void onViewDestroyed();

    void onGameDirectoryChanged(boost::filesystem::path path);

private:
    boost::filesystem::path _gamePath;
    game::GameID _gameId {game::GameID::KotOR};
    std::vector<std::shared_ptr<Tool>> _tools;

    void reloadTools();
};

} // namespace reone
