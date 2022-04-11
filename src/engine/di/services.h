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

#include "../../game/options.h"
#include "../../game/types.h"

#include "module/audio.h"
#include "module/game.h"
#include "module/graphics.h"
#include "module/resource.h"
#include "module/scene.h"
#include "module/script.h"

namespace reone {

class Services {
public:
    Services(game::GameID gameId, game::Options &gameOptions) :
        _gameId(gameId),
        _gameOptions(gameOptions) {
    }

    ~Services() { deinit(); }

    void init();
    void deinit();

    game::ServicesView &view() { return *_view; }

private:
    game::GameID _gameId;
    game::Options &_gameOptions;

    std::unique_ptr<ResourceModule> _resource;
    std::unique_ptr<GraphicsModule> _graphics;
    std::unique_ptr<AudioModule> _audio;
    std::unique_ptr<SceneModule> _scene;
    std::unique_ptr<ScriptModule> _script;
    std::unique_ptr<GameModule> _game;

    std::unique_ptr<game::ServicesView> _view;
};

} // namespace reone
