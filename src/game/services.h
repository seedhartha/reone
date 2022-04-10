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

class CameraStyles;
class Classes;
class Cursors;
class Dialogs;
class Feats;
class FootstepSounds;
class GUISounds;
class Layouts;
class Paths;
class Portraits;
class Reputes;
class ResourceLayout;
class Skills;
class SoundSets;
class Spells;
class Surfaces;
class Visibilities;

struct GameServices {
    CameraStyles &cameraStyles;
    Classes &classes;
    Cursors &cursors;
    Dialogs &dialogs;
    Feats &feats;
    FootstepSounds &footstepSounds;
    GUISounds &guiSounds;
    Layouts &layouts;
    Paths &paths;
    Portraits &portraits;
    Reputes &reputes;
    ResourceLayout &resourceLayout;
    Skills &skills;
    SoundSets &soundSets;
    Spells &spells;
    Surfaces &surfaces;
    Visibilities &visibilities;

    audio::AudioServices &audio;
    graphics::GraphicsServices &graphics;
    scene::SceneServices &scene;
    script::ScriptServices &script;
    resource::ResourceServices &resource;

    GameServices(
        CameraStyles &cameraStyles,
        Classes &classes,
        Cursors &cursors,
        Dialogs &dialogs,
        Feats &feats,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        Layouts &layouts,
        Paths &paths,
        Portraits &portraits,
        Reputes &reputes,
        ResourceLayout &resourceLayout,
        Skills &skills,
        SoundSets &soundSets,
        Spells &spells,
        Surfaces &surfaces,
        Visibilities &visibilities,

        audio::AudioServices &audio,
        graphics::GraphicsServices &graphics,
        scene::SceneServices &scene,
        script::ScriptServices &script,
        resource::ResourceServices &resource) :

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
        visibilities(visibilities),

        audio(audio),
        graphics(graphics),
        scene(scene),
        script(script),
        resource(resource) {
    }
};

} // namespace game

} // namespace reone
