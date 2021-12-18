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

#include "kotor.h"

#include "../../game/types.h"
#include "../../kotor/kotor.h"

#include "audio.h"
#include "graphics.h"
#include "resource.h"
#include "scene.h"
#include "script.h"

using namespace std;

using namespace reone::game;
using namespace reone::kotor;

namespace reone {

void KotorModule::init() {
    _cameraStyles = make_unique<CameraStyles>(_resource.twoDas());
    _classes = make_unique<Classes>(_resource.strings(), _resource.twoDas());
    _cursors = make_unique<Cursors>(_graphics.graphicsContext(), _graphics.meshes(), _graphics.shaders(), _graphics.window(), _resource.resources());
    _dialogs = make_unique<Dialogs>(_resource.gffs(), _resource.strings());
    _feats = make_unique<Feats>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _footstepSounds = make_unique<FootstepSounds>(_audio.audioFiles(), _resource.twoDas());
    _guiSounds = make_unique<GUISounds>(_audio.audioFiles(), _resource.twoDas());
    _layouts = make_unique<Layouts>(_resource.resources());
    _paths = make_unique<Paths>(_resource.gffs());
    _portraits = make_unique<Portraits>(_graphics.textures(), _resource.twoDas());
    _reputes = make_unique<Reputes>(_resource.twoDas());
    _skills = make_unique<Skills>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _soundSets = make_unique<SoundSets>(_audio.audioFiles(), _resource.resources(), _resource.strings());
    _spells = make_unique<Spells>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _surfaces = make_unique<Surfaces>(_resource.twoDas());
    _visibilities = make_unique<Visibilities>(_resource.resources());

    _services = make_unique<Services>(
        *_cameraStyles,
        *_classes,
        *_dialogs,
        *_feats,
        *_footstepSounds,
        *_guiSounds,
        *_cursors,
        *_layouts,
        *_paths,
        *_portraits,
        *_reputes,
        *_skills,
        *_soundSets,
        *_spells,
        *_surfaces,
        *_visibilities,
        _audio.audioContext(),
        _audio.audioFiles(),
        _audio.audioPlayer(),
        _graphics.graphicsContext(),
        _graphics.controlPipeline(),
        _graphics.fonts(),
        _graphics.lips(),
        _graphics.meshes(),
        _graphics.models(),
        _graphics.shaders(),
        _graphics.textures(),
        _graphics.walkmeshes(),
        _graphics.window(),
        _graphics.worldPipeline(),
        _scene.sceneGraphs(),
        _script.scripts(),
        _resource.gffs(),
        _resource.resources(),
        _resource.strings(),
        _resource.twoDas());

    _game = newGame();

    _game->initResourceProviders();
    _cameraStyles->init();
    _feats->init();
    _guiSounds->init();
    _portraits->init();
    _reputes->init();
    _skills->init();
    _spells->init();
    _surfaces->init();
    _game->init();

    _graphics.window().setEventHandler(_game.get());
}

void KotorModule::deinit() {
    _game.reset();
    _services.reset();

    _layouts.reset();
    _cameraStyles.reset();
    _classes.reset();
    _cursors.reset();
    _dialogs.reset();
    _feats.reset();
    _footstepSounds.reset();
    _guiSounds.reset();
    _paths.reset();
    _portraits.reset();
    _reputes.reset();
    _visibilities.reset();
    _skills.reset();
    _soundSets.reset();
    _spells.reset();
    _surfaces.reset();
}

unique_ptr<Game> KotorModule::newGame() {
    switch (_gameId) {
    case GameID::KotOR:
    case GameID::TSL:
        return make_unique<KotOR>(_gameId == GameID::TSL, _gamePath, _gameOptions, *_services);
    default:
        throw logic_error("Unsupported game ID: " + to_string(static_cast<int>(_gameId)));
    }
}

} // namespace reone
