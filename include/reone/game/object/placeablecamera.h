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

#include "reone/resource/format/gffreader.h"

#include "../camera.h"
#include "../object.h"
#include "../schema/git.h"

namespace reone {

namespace game {

class PlaceableCamera : public Object, public Camera {
public:
    PlaceableCamera(
        uint32_t id,
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        Object(
            id,
            ObjectType::Camera,
            std::move(sceneName),
            game,
            services) {
    }

    void loadFromGIT(const schema::GIT_CameraList &git);

    int cameraId() const { return _cameraId; }
    float fieldOfView() const { return _fieldOfView; }

private:
    int _cameraId {0};
    float _fieldOfView {0.0f};

    void loadTransformFromGIT(const schema::GIT_CameraList &git);
};

} // namespace game

} // namespace reone
