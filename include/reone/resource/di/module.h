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

#include "../audioplayer.h"
#include "../provider/2das.h"
#include "../provider/audiofiles.h"
#include "../provider/cursors.h"
#include "../provider/dialogs.h"
#include "../provider/fonts.h"
#include "../provider/gffs.h"
#include "../provider/layouts.h"
#include "../provider/lips.h"
#include "../provider/ltrs.h"
#include "../provider/models.h"
#include "../provider/movies.h"
#include "../provider/paths.h"
#include "../provider/scripts.h"
#include "../provider/soundsets.h"
#include "../provider/textures.h"
#include "../provider/visibilities.h"
#include "../provider/walkmeshes.h"
#include "../resources.h"
#include "../strings.h"

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
    AudioFiles &audioFiles() { return *_audioFiles; }
    AudioPlayer &audioPlayer() { return *_audioPlayer; }
    Cursors &cursors() { return *_cursors; }
    Fonts &fonts() { return *_fonts; }
    Lips &lips() { return *_lips; }
    Models &models() { return *_models; }
    Textures &textures() { return *_textures; }
    Walkmeshes &walkmeshes() { return *_walkmeshes; }
    Ltrs &ltrs() { return *_ltrs; }

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
    std::unique_ptr<AudioFiles> _audioFiles;
    std::unique_ptr<AudioPlayer> _audioPlayer;
    std::unique_ptr<Cursors> _cursors;
    std::unique_ptr<Fonts> _fonts;
    std::unique_ptr<Lips> _lips;
    std::unique_ptr<Models> _models;
    std::unique_ptr<Textures> _textures;
    std::unique_ptr<Walkmeshes> _walkmeshes;
    std::unique_ptr<Dialogs> _dialogs;
    std::unique_ptr<Layouts> _layouts;
    std::unique_ptr<Paths> _paths;
    std::unique_ptr<SoundSets> _soundSets;
    std::unique_ptr<Visibilities> _visibilities;
    std::unique_ptr<Ltrs> _ltrs;

    std::unique_ptr<ResourceServices> _services;
};

} // namespace resource

} // namespace reone
