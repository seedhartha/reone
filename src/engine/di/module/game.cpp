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

#include "game.h"

#include "../../game/types.h"
#include "../../kotor/kotor.h"
#include "../../kotor/tsl.h"

#include "audio.h"
#include "graphics.h"
#include "resource.h"
#include "scene.h"
#include "script.h"

using namespace std;

using namespace reone::game;
using namespace reone::kotor;

namespace reone {

void GameModule::init() {
    _classes = make_unique<Classes>(_resource.strings(), _resource.twoDas());
    _cursors = make_unique<Cursors>(_graphics.context(), _graphics.meshes(), _graphics.shaders(), _graphics.window(), _resource.resources());
    _feats = make_unique<Feats>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _footstepSounds = make_unique<FootstepSounds>(_audio.audioFiles(), _resource.twoDas());
    _guiSounds = make_unique<GUISounds>(_audio.audioFiles(), _resource.twoDas());
    _portraits = make_unique<Portraits>(_graphics.textures(), _resource.twoDas());
    _reputes = make_unique<Reputes>(_resource.twoDas());
    _skills = make_unique<Skills>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _soundSets = make_unique<SoundSets>(_audio.audioFiles(), _resource.resources(), _resource.strings());
    _spells = make_unique<Spells>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _surfaces = make_unique<Surfaces>(_resource.twoDas());

    _services = make_unique<Services>(
        *_classes,
        *_cursors,
        *_feats,
        *_footstepSounds,
        *_guiSounds,
        *_portraits,
        *_reputes,
        *_skills,
        *_soundSets,
        *_surfaces,
        _audio.audioContext(),
        _audio.audioFiles(),
        _audio.audioPlayer(),
        _graphics.context(),
        _graphics.features(),
        _graphics.fonts(),
        _graphics.lips(),
        _graphics.materials(),
        _graphics.meshes(),
        _graphics.models(),
        _graphics.pbrIbl(),
        _graphics.shaders(),
        _graphics.textures(),
        _graphics.walkmeshes(),
        _graphics.window(),
        _scene.controlRenderPipeline(),
        _scene.sceneGraphs(),
        _scene.worldRenderPipeline(),
        _script.scripts(),
        _resource.gffs(),
        _resource.resources(),
        _resource.strings(),
        _resource.twoDas());

    _game = newGame();

    _game->initResourceProviders();
    _surfaces->init();
    _guiSounds->init();
    _reputes->init();
    _skills->init();
    _feats->init();
    _spells->init();
    _portraits->init();
    _game->init();

    _graphics.window().setEventHandler(_game.get());
}

void GameModule::deinit() {
    _game.reset();
    _services.reset();
    _surfaces.reset();
    _spells.reset();
    _soundSets.reset();
    _skills.reset();
    _reputes.reset();
    _portraits.reset();
    _guiSounds.reset();
    _footstepSounds.reset();
    _feats.reset();
    _cursors.reset();
    _classes.reset();
}

unique_ptr<Game> GameModule::newGame() {
    switch (_gameId) {
    case GameID::KotOR:
        return make_unique<KotOR>(_gamePath, _gameOptions, *_services);
    case GameID::TSL:
        return make_unique<TSL>(_gamePath, _gameOptions, *_services);
    default:
        throw logic_error("Unsupported game ID: " + to_string(static_cast<int>(_gameId)));
    }
}

} // namespace reone
