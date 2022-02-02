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

#pragma once

namespace reone {

namespace resource {

class Gffs;
class Resources;
class Strings;
class TwoDas;

} // namespace resource

namespace graphics {

class Fonts;
class GraphicsContext;
class LipAnimations;
class Meshes;
class Models;
class Pipeline;
class Shaders;
class Textures;
class Uniforms;
class Walkmeshes;
class Window;

} // namespace graphics

namespace audio {

class AudioContext;
class AudioFiles;
class AudioPlayer;

} // namespace audio

namespace scene {

class SceneGraphs;

} // namespace scene

namespace script {

class Scripts;

}

namespace game {

class CameraStyles;
class Classes;
class Dialogs;
class Feats;
class FootstepSounds;
class GUISounds;
class ICursors;
class Layouts;
class Paths;
class Portraits;
class Reputes;
class Skills;
class SoundSets;
class Spells;
class Surfaces;
class Visibilities;

struct Services {
    CameraStyles &cameraStyles;
    Classes &classes;
    Dialogs &dialogs;
    Feats &feats;
    FootstepSounds &footstepSounds;
    GUISounds &guiSounds;
    ICursors &cursors;
    Layouts &layouts;
    Paths &paths;
    Portraits &portraits;
    Reputes &reputes;
    Skills &skills;
    SoundSets &soundSets;
    Spells &spells;
    Surfaces &surfaces;
    Visibilities &visibilities;

    audio::AudioContext &audioContext;
    audio::AudioFiles &audioFiles;
    audio::AudioPlayer &audioPlayer;
    graphics::Fonts &fonts;
    graphics::GraphicsContext &graphicsContext;
    graphics::LipAnimations &lipAnimations;
    graphics::Meshes &meshes;
    graphics::Models &models;
    graphics::Pipeline &pipeline;
    graphics::Shaders &shaders;
    graphics::Textures &textures;
    graphics::Uniforms &uniforms;
    graphics::Walkmeshes &walkmeshes;
    graphics::Window &window;
    scene::SceneGraphs &sceneGraphs;
    script::Scripts &scripts;
    resource::Gffs &gffs;
    resource::Resources &resources;
    resource::Strings &strings;
    resource::TwoDas &twoDas;

    Services(
        CameraStyles &cameraStyles,
        Classes &classes,
        Dialogs &dialogs,
        Feats &feats,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        ICursors &cursors,
        Layouts &layouts,
        Paths &paths,
        Portraits &portraits,
        Reputes &reputes,
        Skills &skills,
        SoundSets &soundSets,
        Spells &spells,
        Surfaces &surfaces,
        Visibilities &visibilities,
        audio::AudioContext &audioContext,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Fonts &fonts,
        graphics::GraphicsContext &graphicsContext,
        graphics::LipAnimations &lipAnimations,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::Pipeline &pipeline,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Uniforms &uniforms,
        graphics::Walkmeshes &walkmeshes,
        graphics::Window &window,
        scene::SceneGraphs &sceneGraphs,
        script::Scripts &scripts,
        resource::Gffs &gffs,
        resource::Resources &resources,
        resource::Strings &strings,
        resource::TwoDas &twoDas) :
        cameraStyles(cameraStyles),
        classes(classes),
        dialogs(dialogs),
        feats(feats),
        footstepSounds(footstepSounds),
        guiSounds(guiSounds),
        cursors(cursors),
        layouts(layouts),
        paths(paths),
        portraits(portraits),
        reputes(reputes),
        skills(skills),
        soundSets(soundSets),
        spells(spells),
        surfaces(surfaces),
        visibilities(visibilities),

        audioContext(audioContext),
        audioFiles(audioFiles),
        audioPlayer(audioPlayer),
        fonts(fonts),
        graphicsContext(graphicsContext),
        lipAnimations(lipAnimations),
        meshes(meshes),
        models(models),
        pipeline(pipeline),
        shaders(shaders),
        textures(textures),
        uniforms(uniforms),
        walkmeshes(walkmeshes),
        window(window),
        sceneGraphs(sceneGraphs),
        scripts(scripts),
        gffs(gffs),
        resources(resources),
        strings(strings),
        twoDas(twoDas) {
    }
};

} // namespace game

} // namespace reone
