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

#pragma once

#include "../game/options.h"
#include "../game/types.h"

#include "services/audio.h"
#include "services/common.h"
#include "services/game.h"
#include "services/graphics.h"
#include "services/resource.h"
#include "services/scene.h"
#include "services/script.h"

namespace reone {

namespace di {

class Container {
public:
    Container(game::GameID gameId, game::Options gameOptions) :
        _gameId(gameId),
        _gameOptions(std::move(gameOptions)) {
    }

    void init();

    game::Game &getGame();

private:
    game::GameID _gameId;
    game::Options _gameOptions;

    std::unique_ptr<CommonServices> _commonServices;
    std::unique_ptr<ResourceServices> _resourceServices;
    std::unique_ptr<GraphicsServices> _graphicsServices;
    std::unique_ptr<AudioServices> _audioServices;
    std::unique_ptr<SceneServices> _sceneServices;
    std::unique_ptr<ScriptServices> _scriptServices;
    std::unique_ptr<GameServices> _gameServices;
};

} // namespace di

} // namespace reone