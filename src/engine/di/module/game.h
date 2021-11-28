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

#include "../../game/cursors.h"
#include "../../game/d20/classes.h"
#include "../../game/d20/feats.h"
#include "../../game/d20/skills.h"
#include "../../game/d20/spells.h"
#include "../../game/footstepsounds.h"
#include "../../game/game.h"
#include "../../game/guisounds.h"
#include "../../game/portraits.h"
#include "../../game/reputes.h"
#include "../../game/services.h"
#include "../../game/soundsets.h"
#include "../../game/surfaces.h"

#include "../../types.h"

namespace reone {

namespace di {

class AudioModule;
class GraphicsModule;
class ResourceModule;
class SceneModule;
class ScriptModule;

class GameModule : boost::noncopyable {
public:
    GameModule(
        game::GameID gameId,
        game::Options gameOptions,
        boost::filesystem::path gamePath,
        ResourceModule &resource,
        GraphicsModule &graphics,
        AudioModule &audio,
        SceneModule &scene,
        ScriptModule &script) :
        _gameId(gameId),
        _gamePath(std::move(gamePath)),
        _gameOptions(std::move(gameOptions)),
        _resource(resource),
        _graphics(graphics),
        _audio(audio),
        _scene(scene),
        _script(script) {
    }

    ~GameModule();

    void init();

    game::Game &game() { return *_game; }

private:
    game::GameID _gameId;
    game::Options _gameOptions;
    boost::filesystem::path _gamePath;

    ResourceModule &_resource;
    GraphicsModule &_graphics;
    AudioModule &_audio;
    SceneModule &_scene;
    ScriptModule &_script;

    std::unique_ptr<game::Classes> _classes;
    std::unique_ptr<game::Cursors> _cursors;
    std::unique_ptr<game::Feats> _feats;
    std::unique_ptr<game::FootstepSounds> _footstepSounds;
    std::unique_ptr<game::Game> _game;
    std::unique_ptr<game::GUISounds> _guiSounds;
    std::unique_ptr<game::Portraits> _portraits;
    std::unique_ptr<game::Reputes> _reputes;
    std::unique_ptr<game::Skills> _skills;
    std::unique_ptr<game::SoundSets> _soundSets;
    std::unique_ptr<game::Spells> _spells;
    std::unique_ptr<game::Surfaces> _surfaces;

    std::unique_ptr<game::Services> _services;

    std::unique_ptr<game::Game> newGame();
};

} // namespace di

} // namespace reone
