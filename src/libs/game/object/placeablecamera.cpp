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

#include "reone/game/object/placeablecamera.h"

using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void PlaceableCamera::loadFromGIT(const schema::GIT_CameraList &git) {
    _cameraId = git.CameraID;
    _fieldOfView = git.FieldOfView;

    loadTransformFromGIT(git);
}

void PlaceableCamera::loadTransformFromGIT(const schema::GIT_CameraList &git) {
    glm::vec3 position(git.Position);
    float height = git.Height;

    _position = glm::vec3(position.x, position.y, position.z + height);

    glm::quat orientation(git.Orientation);
    float pitch = git.Pitch;

    _orientation = std::move(orientation);
    _orientation *= glm::quat_cast(glm::eulerAngleX(glm::radians(pitch)));

    updateTransform();
}

} // namespace game

} // namespace reone
