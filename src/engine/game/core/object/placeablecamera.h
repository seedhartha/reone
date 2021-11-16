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

#include "../../../resource/format/gffreader.h"

#include "../camera/camera.h"

namespace reone {

namespace game {

class PlaceableCamera : public SpatialObject, public Camera {
public:
    PlaceableCamera(
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
        resource::TwoDas &twoDas,
        scene::SceneGraph &sceneGraph) :
        SpatialObject(
            id,
            ObjectType::Camera,
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
            twoDas,
            sceneGraph) {
    }

    void loadFromGIT(const resource::GffStruct &gffs);

    int cameraId() const { return _cameraId; }
    float fieldOfView() const { return _fieldOfView; }

private:
    int _cameraId {0};
    float _fieldOfView {0.0f};

    void loadTransformFromGIT(const resource::GffStruct &gffs);
};

} // namespace game

} // namespace reone
