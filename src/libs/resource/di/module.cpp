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

namespace reone {

namespace resource {

void ResourceModule::init() {
    _resources = std::make_unique<Resources>();
    _strings = std::make_unique<Strings>();
    _twoDas = std::make_unique<TwoDas>(*_resources);
    _gffs = std::make_unique<Gffs>(*_resources);
    _scripts = std::make_unique<Scripts>(*_resources);
    _files = std::make_unique<AudioFiles>(*_resources);
    _player = std::make_unique<AudioPlayer>(_audioOpts, *_files);
    _movies = std::make_unique<Movies>(_gamePath, *_player);

    _strings->init(_gamePath);

    _services = std::make_unique<ResourceServices>(
        *_gffs,
        *_resources,
        *_strings,
        *_twoDas,
        *_scripts,
        *_movies,
        *_files,
        *_player);
}

void ResourceModule::deinit() {
    _services.reset();

    _player.reset();
    _files.reset();
    _movies.reset();
    _scripts.reset();
    _gffs.reset();
    _twoDas.reset();
    _strings.reset();
    _resources.reset();
}

} // namespace resource

} // namespace reone
