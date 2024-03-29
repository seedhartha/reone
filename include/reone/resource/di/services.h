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

namespace reone {

namespace resource {

class IAudioClips;
class ICursors;
class IDialogs;
class IFonts;
class IGffs;
class ILayouts;
class ILips;
class ILtrs;
class IModels;
class IMovies;
class IPaths;
class IResourceDirector;
class IResources;
class IScripts;
class IShaders;
class ISoundSets;
class IStrings;
class ITextures;
class ITwoDAs;
class IVisibilities;
class IWalkmeshes;

struct ResourceServices {
    IGffs &gffs;
    IResources &resources;
    IStrings &strings;
    ITwoDAs &twoDas;
    IScripts &scripts;
    IMovies &movies;
    IAudioClips &audioClips;
    ICursors &cursors;
    IFonts &fonts;
    ILips &lips;
    IModels &models;
    ITextures &textures;
    IWalkmeshes &walkmeshes;
    IDialogs &dialogs;
    ILayouts &layouts;
    IPaths &paths;
    ISoundSets &soundSets;
    IVisibilities &visibilities;
    ILtrs &ltrs;
    IShaders &shaders;
    IResourceDirector &director;

    ResourceServices(
        IGffs &gffs,
        IResources &resources,
        IStrings &strings,
        ITwoDAs &twoDas,
        IScripts &scripts,
        IMovies &movies,
        IAudioClips &audioClips,
        ICursors &cursors,
        IFonts &fonts,
        ILips &lips,
        IModels &models,
        ITextures &textures,
        IWalkmeshes &walkmeshes,
        IDialogs &dialogs,
        ILayouts &layouts,
        IPaths &paths,
        ISoundSets &soundSets,
        IVisibilities &visibilities,
        ILtrs &ltrs,
        IShaders &shaders,
        IResourceDirector &director) :
        gffs(gffs),
        resources(resources),
        strings(strings),
        twoDas(twoDas),
        scripts(scripts),
        movies(movies),
        audioClips(audioClips),
        cursors(cursors),
        fonts(fonts),
        lips(lips),
        models(models),
        textures(textures),
        walkmeshes(walkmeshes),
        dialogs(dialogs),
        layouts(layouts),
        paths(paths),
        soundSets(soundSets),
        visibilities(visibilities),
        ltrs(ltrs),
        shaders(shaders),
        director(director) {
    }
};

} // namespace resource

} // namespace reone
