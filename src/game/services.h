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

class Context;
class Fonts;
class Lips;
class Meshes;
class Models;
class Shaders;
class Textures;
class Walkmeshes;
class Window;

} // namespace graphics

namespace audio {

class AudioContext;
class AudioFiles;
class AudioPlayer;

} // namespace audio

namespace scene {

class ControlRenderPipeline;
class SceneGraphs;
class WorldRenderPipeline;

} // namespace scene

namespace script {

class Scripts;

}

namespace game {

class Classes;
class Combat;
class Feats;
class FootstepSounds;
class GUISounds;
class ICursors;
class Portraits;
class Reputes;
class Skills;
class SoundSets;
class Surfaces;

struct Services {
    Classes &classes;
    Feats &feats;
    FootstepSounds &footstepSounds;
    GUISounds &guiSounds;
    ICursors &cursors;
    Portraits &portraits;
    Reputes &reputes;
    Skills &skills;
    SoundSets &soundSets;
    Surfaces &surfaces;

    audio::AudioContext &audioContext;
    audio::AudioFiles &audioFiles;
    audio::AudioPlayer &audioPlayer;
    graphics::Context &context;
    graphics::Fonts &fonts;
    graphics::Lips &lips;
    graphics::Meshes &meshes;
    graphics::Models &models;
    graphics::Shaders &shaders;
    graphics::Textures &textures;
    graphics::Walkmeshes &walkmeshes;
    graphics::Window &window;
    scene::ControlRenderPipeline &controlRenderPipeline;
    scene::SceneGraphs &sceneGraphs;
    scene::WorldRenderPipeline &worldRenderPipeline;
    script::Scripts &scripts;
    resource::Gffs &gffs;
    resource::Resources &resources;
    resource::Strings &strings;
    resource::TwoDas &twoDas;

    Services(
        Classes &classes,
        Feats &feats,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        ICursors &cursors,
        Portraits &portraits,
        Reputes &reputes,
        Skills &skills,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioContext &audioContext,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Fonts &fonts,
        graphics::Lips &lips,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        graphics::Window &window,
        scene::ControlRenderPipeline &controlRenderPipeline,
        scene::SceneGraphs &sceneGraphs,
        scene::WorldRenderPipeline &worldRenderPipeline,
        script::Scripts &scripts,
        resource::Gffs &gffs,
        resource::Resources &resources,
        resource::Strings &strings,
        resource::TwoDas &twoDas) :
        classes(classes),
        feats(feats),
        footstepSounds(footstepSounds),
        guiSounds(guiSounds),
        cursors(cursors),
        portraits(portraits),
        reputes(reputes),
        skills(skills),
        soundSets(soundSets),
        surfaces(surfaces),

        audioContext(audioContext),
        audioFiles(audioFiles),
        audioPlayer(audioPlayer),
        context(context),
        fonts(fonts),
        lips(lips),
        meshes(meshes),
        models(models),
        shaders(shaders),
        textures(textures),
        walkmeshes(walkmeshes),
        window(window),
        controlRenderPipeline(controlRenderPipeline),
        sceneGraphs(sceneGraphs),
        worldRenderPipeline(worldRenderPipeline),
        scripts(scripts),
        gffs(gffs),
        resources(resources),
        strings(strings),
        twoDas(twoDas) {
    }
};

} // namespace game

} // namespace reone
