/*
 * Copyright (c) 2020-2022 The reone project contributors
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

class AStar;
class CameraStyles;
class Cursors;
class Dialogs;
class FootstepSounds;
class GUISounds;
class Layouts;
class Paths;
class Portraits;
class ResourceLayout;
class SoundSets;
class Surfaces;
class Visibilities;

struct GameServices {
    AStar &aStar;
    CameraStyles &cameraStyles;
    Cursors &cursors;
    FootstepSounds &footstepSounds;
    GUISounds &guiSounds;
    Layouts &layouts;
    Paths &paths;
    Portraits &portraits;
    ResourceLayout &resourceLayout;
    SoundSets &soundSets;
    Surfaces &surfaces;
    Visibilities &visibilities;

    GameServices(
        AStar &aStar,
        CameraStyles &cameraStyles,
        Cursors &cursors,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        Layouts &layouts,
        Paths &paths,
        Portraits &portraits,
        ResourceLayout &resourceLayout,
        SoundSets &soundSets,
        Surfaces &surfaces,
        Visibilities &visibilities) :
        aStar(aStar),
        cameraStyles(cameraStyles),
        cursors(cursors),
        footstepSounds(footstepSounds),
        guiSounds(guiSounds),
        layouts(layouts),
        paths(paths),
        portraits(portraits),
        resourceLayout(resourceLayout),
        soundSets(soundSets),
        surfaces(surfaces),
        visibilities(visibilities) {
    }
};

struct ServicesView {
    GameServices &game;
    audio::AudioServices &audio;
    graphics::GraphicsServices &graphics;
    scene::SceneServices &scene;
    script::ScriptServices &script;
    resource::ResourceServices &resource;

    ServicesView(
        GameServices &game,
        audio::AudioServices &audio,
        graphics::GraphicsServices &graphics,
        scene::SceneServices &scene,
        script::ScriptServices &script,
        resource::ResourceServices &resource) :
        game(game),
        audio(audio),
        graphics(graphics),
        scene(scene),
        script(script),
        resource(resource) {
    }
};

} // namespace game

} // namespace reone
