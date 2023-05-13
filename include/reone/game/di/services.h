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
#include "reone/movie/di/services.h"
#include "reone/resource/di/services.h"
#include "reone/scene/di/services.h"
#include "reone/script/di/services.h"
#include "reone/system/di/services.h"

#include "../camerastyles.h"
#include "../cursors.h"
#include "../d20/classes.h"
#include "../d20/feats.h"
#include "../d20/skills.h"
#include "../d20/spells.h"
#include "../dialogs.h"
#include "../footstepsounds.h"
#include "../guisounds.h"
#include "../layouts.h"
#include "../paths.h"
#include "../portraits.h"
#include "../reputes.h"
#include "../resourcelayout.h"
#include "../soundsets.h"
#include "../surfaces.h"
#include "../visibilities.h"

namespace reone {

namespace game {

struct GameServices {
    ICameraStyles &cameraStyles;
    IClasses &classes;
    ICursors &cursors;
    IDialogs &dialogs;
    IFeats &feats;
    IFootstepSounds &footstepSounds;
    IGUISounds &guiSounds;
    ILayouts &layouts;
    IPaths &paths;
    IPortraits &portraits;
    IReputes &reputes;
    IResourceLayout &resourceLayout;
    ISkills &skills;
    ISoundSets &soundSets;
    ISpells &spells;
    ISurfaces &surfaces;
    IVisibilities &visibilities;

    GameServices(
        ICameraStyles &cameraStyles,
        IClasses &classes,
        ICursors &cursors,
        IDialogs &dialogs,
        IFeats &feats,
        IFootstepSounds &footstepSounds,
        IGUISounds &guiSounds,
        ILayouts &layouts,
        IPaths &paths,
        IPortraits &portraits,
        IReputes &reputes,
        IResourceLayout &resourceLayout,
        ISkills &skills,
        ISoundSets &soundSets,
        ISpells &spells,
        ISurfaces &surfaces,
        IVisibilities &visibilities) :
        cameraStyles(cameraStyles),
        classes(classes),
        cursors(cursors),
        dialogs(dialogs),
        feats(feats),
        footstepSounds(footstepSounds),
        guiSounds(guiSounds),
        layouts(layouts),
        paths(paths),
        portraits(portraits),
        reputes(reputes),
        resourceLayout(resourceLayout),
        skills(skills),
        soundSets(soundSets),
        spells(spells),
        surfaces(surfaces),
        visibilities(visibilities) {
    }
};

struct ServicesView {
    GameServices &game;
    movie::MovieServices &movie;
    audio::AudioServices &audio;
    graphics::GraphicsServices &graphics;
    scene::SceneServices &scene;
    script::ScriptServices &script;
    resource::ResourceServices &resource;
    SystemServices &system;

    ServicesView(
        GameServices &game,
        movie::MovieServices &movie,
        audio::AudioServices &audio,
        graphics::GraphicsServices &graphics,
        scene::SceneServices &scene,
        script::ScriptServices &script,
        resource::ResourceServices &resource,
        SystemServices &system) :
        game(game),
        movie(movie),
        audio(audio),
        graphics(graphics),
        scene(scene),
        script(script),
        resource(resource),
        system(system) {
    }
};

} // namespace game

} // namespace reone
