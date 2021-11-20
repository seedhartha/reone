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
class Features;
class Fonts;
class Lips;
class Materials;
class Meshes;
class Models;
class PBRIBL;
class Shaders;
class Textures;
class Walkmeshes;
class Window;

} // namespace graphics

namespace audio {

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

class ActionFactory;
class Classes;
class Combat;
class Cursors;
class EffectFactory;
class Feats;
class FootstepSounds;
class GUISounds;
class ObjectFactory;
class Party;
class Portraits;
class Reputes;
class SceneManager;
class ScriptRunner;
class Skills;
class SoundSets;
class Surfaces;

struct Services {
    ActionFactory &actionFactory;
    Classes &classes;
    Combat &combat;
    Cursors &cursors;
    EffectFactory &effectFactory;
    Feats &feats;
    FootstepSounds &footstepSounds;
    GUISounds &guiSounds;
    ObjectFactory &objectFactory;
    Party &party;
    Portraits &portraits;
    Reputes &reputes;
    SceneManager &sceneManager;
    ScriptRunner &scriptRunner;
    Skills &skills;
    SoundSets &soundSets;
    Surfaces &surfaces;

    audio::AudioFiles &audioFiles;
    audio::AudioPlayer &audioPlayer;
    graphics::Context &context;
    graphics::Features &features;
    graphics::Fonts &fonts;
    graphics::Lips &lips;
    graphics::Materials &materials;
    graphics::Meshes &meshes;
    graphics::Models &models;
    graphics::PBRIBL &pbrIbl;
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
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        Cursors &cursors,
        EffectFactory &effectFactory,
        Feats &feats,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        SceneManager &sceneManager,
        ScriptRunner &scriptRunner,
        Skills &skills,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Features &features,
        graphics::Fonts &fonts,
        graphics::Lips &lips,
        graphics::Materials &materials,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::PBRIBL &pbrIbl,
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
        actionFactory(actionFactory),
        classes(classes),
        combat(combat),
        cursors(cursors),
        effectFactory(effectFactory),
        feats(feats),
        footstepSounds(footstepSounds),
        guiSounds(guiSounds),
        objectFactory(objectFactory),
        party(party),
        portraits(portraits),
        reputes(reputes),
        sceneManager(sceneManager),
        scriptRunner(scriptRunner),
        skills(skills),
        soundSets(soundSets),
        surfaces(surfaces),

        audioFiles(audioFiles),
        audioPlayer(audioPlayer),
        context(context),
        features(features),
        fonts(fonts),
        lips(lips),
        materials(materials),
        meshes(meshes),
        models(models),
        pbrIbl(pbrIbl),
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
