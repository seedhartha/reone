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

#include "camerastyles.h"
#include "cursors.h"
#include "d20/classes.h"
#include "d20/feats.h"
#include "d20/skills.h"
#include "d20/spells.h"
#include "dialogs.h"
#include "footstepsounds.h"
#include "guisounds.h"
#include "layouts.h"
#include "paths.h"
#include "portraits.h"
#include "reputes.h"
#include "resourcelayout.h"
#include "soundsets.h"
#include "surfaces.h"
#include "visibilities.h"

namespace reone {

struct SystemServices;

namespace resource {

struct ResourceServices;

}

namespace audio {

struct AudioServices;

}

namespace graphics {

struct GraphicsServices;

}

namespace scene {

struct SceneServices;

}

namespace script {

struct ScriptServices;

}

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

    CameraStyles &defaultCameraStyles() {
        auto casted = dynamic_cast<CameraStyles *>(&cameraStyles);
        if (!casted) {
            throw std::logic_error("Illegal CameraStyles implementation");
        }
        return *casted;
    }

    Classes &defaultClasses() {
        auto casted = dynamic_cast<Classes *>(&classes);
        if (!casted) {
            throw std::logic_error("Illegal Classes implementation");
        }
        return *casted;
    }

    Cursors &defaultCursors() {
        auto casted = dynamic_cast<Cursors *>(&cursors);
        if (!casted) {
            throw std::logic_error("Illegal Cursors implementation");
        }
        return *casted;
    }

    Dialogs &defaultDialogs() {
        auto casted = dynamic_cast<Dialogs *>(&dialogs);
        if (!casted) {
            throw std::logic_error("Illegal Dialogs implementation");
        }
        return *casted;
    }

    Feats &defaultFeats() {
        auto casted = dynamic_cast<Feats *>(&feats);
        if (!casted) {
            throw std::logic_error("Illegal Feats implementation");
        }
        return *casted;
    }

    FootstepSounds &defaultFootstepSounds() {
        auto casted = dynamic_cast<FootstepSounds *>(&footstepSounds);
        if (!casted) {
            throw std::logic_error("Illegal FootstepSounds implementation");
        }
        return *casted;
    }

    GUISounds &defaultGUISounds() {
        auto casted = dynamic_cast<GUISounds *>(&guiSounds);
        if (!casted) {
            throw std::logic_error("Illegal GUISounds implementation");
        }
        return *casted;
    }

    Layouts &defaultLayouts() {
        auto casted = dynamic_cast<Layouts *>(&layouts);
        if (!casted) {
            throw std::logic_error("Illegal Layouts implementation");
        }
        return *casted;
    }

    Paths &defaultPaths() {
        auto casted = dynamic_cast<Paths *>(&paths);
        if (!casted) {
            throw std::logic_error("Illegal Paths implementation");
        }
        return *casted;
    }

    Portraits &defaultPortraits() {
        auto casted = dynamic_cast<Portraits *>(&portraits);
        if (!casted) {
            throw std::logic_error("Illegal Portraits implementation");
        }
        return *casted;
    }

    Reputes &defaultReputes() {
        auto casted = dynamic_cast<Reputes *>(&reputes);
        if (!casted) {
            throw std::logic_error("Illegal Reputes implementation");
        }
        return *casted;
    }

    ResourceLayout &defaultResourceLayout() {
        auto casted = dynamic_cast<ResourceLayout *>(&resourceLayout);
        if (!casted) {
            throw std::logic_error("Illegal ResourceLayout implementation");
        }
        return *casted;
    }

    Skills &defaultSkills() {
        auto casted = dynamic_cast<Skills *>(&skills);
        if (!casted) {
            throw std::logic_error("Illegal Skills implementation");
        }
        return *casted;
    }

    SoundSets &defaultSoundSets() {
        auto casted = dynamic_cast<SoundSets *>(&soundSets);
        if (!casted) {
            throw std::logic_error("Illegal SoundSets implementation");
        }
        return *casted;
    }

    Spells &defaultSpells() {
        auto casted = dynamic_cast<Spells *>(&spells);
        if (!casted) {
            throw std::logic_error("Illegal Spells implementation");
        }
        return *casted;
    }

    Surfaces &defaultSurfaces() {
        auto casted = dynamic_cast<Surfaces *>(&surfaces);
        if (!casted) {
            throw std::logic_error("Illegal Surfaces implementation");
        }
        return *casted;
    }

    Visibilities &defaultVisibilities() {
        auto casted = dynamic_cast<Visibilities *>(&visibilities);
        if (!casted) {
            throw std::logic_error("Illegal Visibilities implementation");
        }
        return *casted;
    }
};

struct ServicesView {
    GameServices &game;
    audio::AudioServices &audio;
    graphics::GraphicsServices &graphics;
    scene::SceneServices &scene;
    script::ScriptServices &script;
    resource::ResourceServices &resource;
    SystemServices &system;

    ServicesView(
        GameServices &game,
        audio::AudioServices &audio,
        graphics::GraphicsServices &graphics,
        scene::SceneServices &scene,
        script::ScriptServices &script,
        resource::ResourceServices &resource,
        SystemServices &system) :
        game(game),
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
