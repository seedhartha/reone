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

#include "reone/game/camera/static.h"

#include "reone/scene/graph.h"

#include "reone/game/object/camera.h"

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

StaticCamera::StaticCamera(float aspect, ISceneGraph &sceneGraph) :
    _aspect(aspect) {

    _sceneNode = sceneGraph.newCamera();
    _sceneNode->setPerspectiveProjection(glm::radians(55.0f), aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);
}

void StaticCamera::setObject(const CameraObject &object) {
    _sceneNode->setLocalTransform(object.transform());
    _sceneNode->setPerspectiveProjection(glm::radians(object.fieldOfView()), _aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);
}

} // namespace game

} // namespace reone
