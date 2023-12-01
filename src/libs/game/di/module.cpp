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

namespace reone {

namespace game {

void GameModule::init() {
    _cameraStyles = std::make_unique<CameraStyles>(_resource.twoDas());
    _classes = std::make_unique<Classes>(_resource.strings(), _resource.twoDas());
    _dialogs = std::make_unique<Dialogs>(_resource.gffs(), _resource.strings());
    _feats = std::make_unique<Feats>(_resource.textures(), _resource.strings(), _resource.twoDas());
    _footstepSounds = std::make_unique<FootstepSounds>(_resource.files(), _resource.twoDas());
    _guiSounds = std::make_unique<GUISounds>(_resource.files(), _resource.twoDas());
    _layouts = std::make_unique<Layouts>(_resource.resources());
    _paths = std::make_unique<Paths>(_resource.gffs());
    _portraits = std::make_unique<Portraits>(_resource.textures(), _resource.twoDas());
    _reputes = std::make_unique<Reputes>(_resource.twoDas());
    _resourceDirector = std::make_unique<ResourceDirector>(_gameId, _options, *_dialogs, *_paths, _script.services(), _graphics.services(), _resource.services());
    _skills = std::make_unique<Skills>(_resource.textures(), _resource.strings(), _resource.twoDas());
    _soundSets = std::make_unique<SoundSets>(_resource.files(), _resource.resources(), _resource.strings());
    _spells = std::make_unique<Spells>(_resource.textures(), _resource.strings(), _resource.twoDas());
    _surfaces = std::make_unique<Surfaces>(_resource.twoDas());
    _visibilities = std::make_unique<Visibilities>(_resource.resources());

    _services = std::make_unique<GameServices>(
        *_cameraStyles,
        *_classes,
        *_dialogs,
        *_feats,
        *_footstepSounds,
        *_guiSounds,
        *_layouts,
        *_paths,
        *_portraits,
        *_reputes,
        *_resourceDirector,
        *_skills,
        *_soundSets,
        *_spells,
        *_surfaces,
        *_visibilities);

    _resourceDirector->init();
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
    _resourceDirector.reset();
    _portraits.reset();
    _paths.reset();
    _layouts.reset();
    _guiSounds.reset();
    _footstepSounds.reset();
    _cameraStyles.reset();
}

} // namespace game

} // namespace reone
