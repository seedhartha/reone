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

#include "reone/audio/di/services.h"
#include "reone/graphics/di/services.h"
#include "reone/gui/di/services.h"
#include "reone/movie/di/services.h"
#include "reone/resource/di/services.h"
#include "reone/scene/di/services.h"
#include "reone/script/di/services.h"
#include "reone/system/di/services.h"

namespace reone {

namespace game {

class ICameraStyles;
class IClasses;
class IFeats;
class IFootstepSounds;
class IGUISounds;
class IPortraits;
class IReputes;
class ISkills;
class ISpells;
class ISurfaces;

struct GameServices {
    ICameraStyles &cameraStyles;
    IClasses &classes;
    IFeats &feats;
    IFootstepSounds &footstepSounds;
    IGUISounds &guiSounds;
    IPortraits &portraits;
    IReputes &reputes;
    ISkills &skills;
    ISpells &spells;
    ISurfaces &surfaces;

    GameServices(
        ICameraStyles &cameraStyles,
        IClasses &classes,
        IFeats &feats,
        IFootstepSounds &footstepSounds,
        IGUISounds &guiSounds,
        IPortraits &portraits,
        IReputes &reputes,
        ISkills &skills,
        ISpells &spells,
        ISurfaces &surfaces) :
        cameraStyles(cameraStyles),
        classes(classes),
        feats(feats),
        footstepSounds(footstepSounds),
        guiSounds(guiSounds),
        portraits(portraits),
        reputes(reputes),
        skills(skills),
        spells(spells),
        surfaces(surfaces) {
    }
};

struct ServicesView {
    GameServices &game;
    movie::MovieServices &movie;
    audio::AudioServices &audio;
    graphics::GraphicsServices &graphics;
    scene::SceneServices &scene;
    gui::GUIServices &gui;
    script::ScriptServices &script;
    resource::ResourceServices &resource;
    SystemServices &system;

    ServicesView(
        GameServices &game,
        movie::MovieServices &movie,
        audio::AudioServices &audio,
        graphics::GraphicsServices &graphics,
        scene::SceneServices &scene,
        gui::GUIServices &gui,
        script::ScriptServices &script,
        resource::ResourceServices &resource,
        SystemServices &system) :
        game(game),
        movie(movie),
        audio(audio),
        graphics(graphics),
        scene(scene),
        gui(gui),
        script(script),
        resource(resource),
        system(system) {
    }
};

} // namespace game

} // namespace reone
