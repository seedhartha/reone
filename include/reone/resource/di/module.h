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
#include "../cursors.h"
#include "../fonts.h"
#include "../gffs.h"
#include "../lips.h"
#include "../models.h"
#include "../movies.h"
#include "../resources.h"
#include "../scripts.h"
#include "../strings.h"
#include "../textures.h"
#include "../walkmeshes.h"

#include "services.h"

namespace reone {

namespace graphics {

class GraphicsModule;

}

namespace resource {

class ResourceModule : boost::noncopyable {
public:
    ResourceModule(std::filesystem::path gamePath,
                   graphics::GraphicsOptions &graphicsOpt,
                   audio::AudioOptions &audioOpt,
                   graphics::GraphicsModule &graphics) :
        _gamePath(std::move(gamePath)),
        _graphicsOpt(graphicsOpt),
        _audioOpt(audioOpt),
        _graphics(graphics) {
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
    Cursors &cursors() { return *_cursors; }
    Fonts &fonts() { return *_fonts; }
    Lips &lips() { return *_lips; }
    Models &models() { return *_models; }
    Textures &textures() { return *_textures; }
    Walkmeshes &walkmeshes() { return *_walkmeshes; }

    ResourceServices &services() { return *_services; }

private:
    std::filesystem::path _gamePath;
    graphics::GraphicsOptions &_graphicsOpt;
    audio::AudioOptions &_audioOpt;
    graphics::GraphicsModule &_graphics;

    std::unique_ptr<Gffs> _gffs;
    std::unique_ptr<Resources> _resources;
    std::unique_ptr<Strings> _strings;
    std::unique_ptr<TwoDas> _twoDas;
    std::unique_ptr<Scripts> _scripts;
    std::unique_ptr<Movies> _movies;
    std::unique_ptr<AudioFiles> _files;
    std::unique_ptr<AudioPlayer> _player;
    std::unique_ptr<Cursors> _cursors;
    std::unique_ptr<Fonts> _fonts;
    std::unique_ptr<Lips> _lips;
    std::unique_ptr<Models> _models;
    std::unique_ptr<Textures> _textures;
    std::unique_ptr<Walkmeshes> _walkmeshes;

    std::unique_ptr<ResourceServices> _services;
};

} // namespace resource

} // namespace reone
