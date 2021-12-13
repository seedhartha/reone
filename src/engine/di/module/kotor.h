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

#include "../../game/game.h"
#include "../../game/services.h"
#include "../../kotor/arealayouts.h"
#include "../../kotor/camerastyles.h"
#include "../../kotor/cursors.h"
#include "../../kotor/d20/classes.h"
#include "../../kotor/d20/feats.h"
#include "../../kotor/d20/skills.h"
#include "../../kotor/d20/spells.h"
#include "../../kotor/dialogs.h"
#include "../../kotor/footstepsounds.h"
#include "../../kotor/gui/sounds.h"
#include "../../kotor/paths.h"
#include "../../kotor/portraits.h"
#include "../../kotor/reputes.h"
#include "../../kotor/soundsets.h"
#include "../../kotor/surfaces.h"
#include "../../kotor/visibilities.h"

#include "../../types.h"

namespace reone {

class AudioModule;
class GraphicsModule;
class ResourceModule;
class SceneModule;
class ScriptModule;

class KotorModule : boost::noncopyable {
public:
    KotorModule(
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

    ~KotorModule() { deinit(); }

    void init();
    void deinit();

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

    std::unique_ptr<kotor::AreaLayouts> _areaLayouts;
    std::unique_ptr<kotor::CameraStyles> _cameraStyles;
    std::unique_ptr<kotor::Classes> _classes;
    std::unique_ptr<kotor::Cursors> _cursors;
    std::unique_ptr<kotor::Dialogs> _dialogs;
    std::unique_ptr<kotor::Feats> _feats;
    std::unique_ptr<kotor::FootstepSounds> _footstepSounds;
    std::unique_ptr<kotor::GUISounds> _guiSounds;
    std::unique_ptr<kotor::Paths> _paths;
    std::unique_ptr<kotor::Portraits> _portraits;
    std::unique_ptr<kotor::Reputes> _reputes;
    std::unique_ptr<kotor::Skills> _skills;
    std::unique_ptr<kotor::SoundSets> _soundSets;
    std::unique_ptr<kotor::Spells> _spells;
    std::unique_ptr<kotor::Surfaces> _surfaces;
    std::unique_ptr<kotor::Visibilities> _visibilities;

    std::unique_ptr<game::Services> _services;
    std::unique_ptr<game::Game> _game;

    std::unique_ptr<game::Game> newGame();
};

} // namespace reone
