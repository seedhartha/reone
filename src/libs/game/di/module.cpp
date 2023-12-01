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
    _feats = std::make_unique<Feats>(_resource.textures(), _resource.strings(), _resource.twoDas());
    _footstepSounds = std::make_unique<FootstepSounds>(_resource.audioFiles(), _resource.twoDas());
    _guiSounds = std::make_unique<GUISounds>(_resource.audioFiles(), _resource.twoDas());
    _portraits = std::make_unique<Portraits>(_resource.textures(), _resource.twoDas());
    _reputes = std::make_unique<Reputes>(_resource.twoDas());
    _skills = std::make_unique<Skills>(_resource.textures(), _resource.strings(), _resource.twoDas());
    _spells = std::make_unique<Spells>(_resource.textures(), _resource.strings(), _resource.twoDas());
    _surfaces = std::make_unique<Surfaces>(_resource.twoDas());

    _services = std::make_unique<GameServices>(
        *_cameraStyles,
        *_classes,
        *_feats,
        *_footstepSounds,
        *_guiSounds,
        *_portraits,
        *_reputes,
        *_skills,
        *_spells,
        *_surfaces);

    _cameraStyles->init();
    _guiSounds->init();
    _portraits->init();
    _surfaces->init();
}

void GameModule::deinit() {
    _services.reset();

    _surfaces.reset();
    _portraits.reset();
    _guiSounds.reset();
    _footstepSounds.reset();
    _cameraStyles.reset();
}

} // namespace game

} // namespace reone
