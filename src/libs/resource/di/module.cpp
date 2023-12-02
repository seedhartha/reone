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

#include "reone/resource/di/module.h"

#include "reone/audio/di/module.h"
#include "reone/graphics/di/module.h"
#include "reone/script/di/module.h"

namespace reone {

namespace resource {

void ResourceModule::init() {
    _resources = std::make_unique<Resources>();
    _strings = std::make_unique<Strings>();
    _twoDas = std::make_unique<TwoDas>(*_resources);
    _gffs = std::make_unique<Gffs>(*_resources);
    _shaders = std::make_unique<Shaders>(_graphicsOpt, _graphics.shaderRegistry());
    _textures = std::make_unique<Textures>(_graphicsOpt, *_resources);
    _models = std::make_unique<Models>(*_textures, *_resources);
    _walkmeshes = std::make_unique<Walkmeshes>(*_resources);
    _lips = std::make_unique<Lips>(*_resources);
    _fonts = std::make_unique<Fonts>(
        _graphics.context(),
        _graphics.meshRegistry(),
        _graphics.shaderRegistry(),
        *_textures,
        _graphics.uniforms());
    _cursors = std::make_unique<Cursors>(
        _graphics.context(),
        _graphics.meshRegistry(),
        _graphics.shaderRegistry(),
        *_textures,
        _graphics.uniforms(),
        _graphics.window(),
        *_resources);
    _audioClips = std::make_unique<AudioClips>(*_resources);
    _movies = std::make_unique<Movies>(_gamePath, _graphics.services(), _audio.player());
    _scripts = std::make_unique<Scripts>(*_resources);
    _dialogs = std::make_unique<Dialogs>(*_gffs, *_strings);
    _layouts = std::make_unique<Layouts>(*_resources);
    _paths = std::make_unique<Paths>(*_gffs);
    _soundSets = std::make_unique<SoundSets>(*_audioClips, *_resources, *_strings);
    _visibilities = std::make_unique<Visibilities>(*_resources);
    _ltrs = std::make_unique<Ltrs>(*_resources);
    _director = std::make_unique<ResourceDirector>(
        _gameId,
        _gamePath,
        _graphicsOpt,
        _graphics.services(),
        _script.services(),
        *_dialogs,
        *_gffs,
        *_lips,
        *_paths,
        *_resources,
        *_scripts);

    _director->init();
    _strings->init(_gamePath);
    _shaders->init();
    _textures->init();

    _services = std::make_unique<ResourceServices>(
        *_gffs,
        *_resources,
        *_strings,
        *_twoDas,
        *_scripts,
        *_movies,
        *_audioClips,
        *_cursors,
        *_fonts,
        *_lips,
        *_models,
        *_textures,
        *_walkmeshes,
        *_dialogs,
        *_layouts,
        *_paths,
        *_soundSets,
        *_visibilities,
        *_ltrs,
        *_shaders,
        *_director);
}

void ResourceModule::deinit() {
    _services.reset();

    _director.reset();
    _ltrs.reset();
    _visibilities.reset();
    _soundSets.reset();
    _paths.reset();
    _layouts.reset();
    _dialogs.reset();
    _scripts.reset();
    _movies.reset();
    _audioClips.reset();
    _cursors.reset();
    _fonts.reset();
    _lips.reset();
    _walkmeshes.reset();
    _models.reset();
    _textures.reset();
    _shaders.reset();
    _gffs.reset();
    _twoDas.reset();
    _strings.reset();
    _resources.reset();
}

} // namespace resource

} // namespace reone
