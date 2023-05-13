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

#include "reone/game/di/module.h"

using namespace std;

namespace reone {

namespace game {

void GameModule::init() {
    _cameraStyles = make_unique<CameraStyles>(_resource.twoDas());
    _classes = make_unique<Classes>(_resource.strings(), _resource.twoDas());
    _cursors = make_unique<Cursors>(_graphics.graphicsContext(), _graphics.meshes(), _graphics.shaders(), _graphics.textures(), _graphics.uniforms(), _graphics.window(), _resource.resources());
    _dialogs = make_unique<Dialogs>(_resource.gffs(), _resource.strings());
    _feats = make_unique<Feats>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _footstepSounds = make_unique<FootstepSounds>(_audio.audioFiles(), _resource.twoDas());
    _guiSounds = make_unique<GUISounds>(_audio.audioFiles(), _resource.twoDas());
    _layouts = make_unique<Layouts>(_resource.resources());
    _paths = make_unique<Paths>(_resource.gffs());
    _portraits = make_unique<Portraits>(_graphics.textures(), _resource.twoDas());
    _reputes = make_unique<Reputes>(_resource.twoDas());
    _resourceLayout = make_unique<ResourceLayout>(_gameId, _options, _resource.services());
    _skills = make_unique<Skills>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _soundSets = make_unique<SoundSets>(_audio.audioFiles(), _resource.resources(), _resource.strings());
    _spells = make_unique<Spells>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _surfaces = make_unique<Surfaces>(_resource.twoDas());
    _visibilities = make_unique<Visibilities>(_resource.resources());

    _services = make_unique<GameServices>(
        *_cameraStyles,
        *_classes,
        *_cursors,
        *_dialogs,
        *_feats,
        *_footstepSounds,
        *_guiSounds,
        *_layouts,
        *_paths,
        *_portraits,
        *_reputes,
        *_resourceLayout,
        *_skills,
        *_soundSets,
        *_spells,
        *_surfaces,
        *_visibilities);

    _resourceLayout->init();
    _cameraStyles->init();
    _guiSounds->init();
    _portraits->init();
    _surfaces->init();
}

void GameModule::deinit() {
    _services.reset();

    _visibilities.reset();
    _surfaces.reset();
    _soundSets.reset();
    _resourceLayout.reset();
    _portraits.reset();
    _paths.reset();
    _layouts.reset();
    _guiSounds.reset();
    _footstepSounds.reset();
    _cursors.reset();
    _cameraStyles.reset();
}

} // namespace game

} // namespace reone
