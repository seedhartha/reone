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

#include "reone/game/object/camera/static.h"

#include "reone/game/di/services.h"
#include "reone/game/object/camera.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

void StaticCamera::loadFromGIT(const schema::GIT_CameraList &git) {
    _cameraId = git.CameraID;
    _fieldOfView = git.FieldOfView;

    auto &scene = _services.scene.graphs.get(_sceneName);
    _sceneNode = scene.newCamera();
    cameraSceneNode()->setPerspectiveProjection(glm::radians(_fieldOfView), _aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);

    loadTransformFromGIT(git);
}

void StaticCamera::loadTransformFromGIT(const schema::GIT_CameraList &git) {
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
