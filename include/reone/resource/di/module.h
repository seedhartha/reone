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

#pragma once

#include "../2das.h"
#include "../audio/files.h"
#include "../audio/player.h"
#include "../gffs.h"
#include "../movies.h"
#include "../resources.h"
#include "../scripts.h"
#include "../strings.h"

#include "services.h"

namespace reone {

namespace resource {

class ResourceModule : boost::noncopyable {
public:
    ResourceModule(std::filesystem::path gamePath,
                   audio::AudioOptions &audioOpts) :
        _gamePath(std::move(gamePath)),
        _audioOpts(audioOpts) {
    }

    ~ResourceModule() { deinit(); }

    void init();
    void deinit();

    Gffs &gffs() { return *_gffs; }
    Resources &resources() { return *_resources; }
    Strings &strings() { return *_strings; }
    TwoDas &twoDas() { return *_twoDas; }
    Scripts &scripts() { return *_scripts; }
    Movies &movies() { return *_movies; }
    AudioFiles &files() { return *_files; }
    AudioPlayer &player() { return *_player; }

    ResourceServices &services() { return *_services; }

private:
    std::filesystem::path _gamePath;
    audio::AudioOptions &_audioOpts;

    std::unique_ptr<Gffs> _gffs;
    std::unique_ptr<Resources> _resources;
    std::unique_ptr<Strings> _strings;
    std::unique_ptr<TwoDas> _twoDas;
    std::unique_ptr<Scripts> _scripts;
    std::unique_ptr<Movies> _movies;
    std::unique_ptr<AudioFiles> _files;
    std::unique_ptr<AudioPlayer> _player;

    std::unique_ptr<ResourceServices> _services;
};

} // namespace resource

} // namespace reone
