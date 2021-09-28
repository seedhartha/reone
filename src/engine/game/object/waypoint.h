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

#include "spatial.h"

#include "../../resource/format/gffreader.h"

namespace reone {

namespace game {

class Waypoint : public SpatialObject {
public:
    Waypoint(
        uint32_t id,
        Game *game,
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        FootstepSounds &footstepSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        resource::Resources &resources,
        resource::Strings &strings,
        scene::SceneGraph &sceneGraph) :
        SpatialObject(
            id,
            ObjectType::Waypoint,
            game,
            actionFactory,
            classes,
            combat,
            footstepSounds,
            objectFactory,
            party,
            portraits,
            reputes,
            scriptRunner,
            soundSets,
            surfaces,
            audioFiles,
            audioPlayer,
            context,
            meshes,
            models,
            shaders,
            textures,
            walkmeshes,
            resources,
            strings,
            sceneGraph) {
    }

    void loadFromGIT(const resource::GffStruct &gffs);
    void loadFromBlueprint(const std::string &resRef);

    bool isMapNoteEnabled() const { return _mapNoteEnabled; }

    const std::string &mapNote() const { return _mapNote; }

private:
    int _appearance {0};
    bool _hasMapNote {false};
    std::string _mapNote;
    bool _mapNoteEnabled {false};

    void loadTransformFromGIT(const resource::GffStruct &gffs);

    void loadUTW(const resource::GffStruct &utw);
};

} // namespace game

} // namespace reone
