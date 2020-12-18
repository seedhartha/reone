/*
 * Copyright (c) 2020 The reone project contributors
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

#include "staticcamera.h"

#include "glm/ext.hpp"

#include "../object/placeablecamera.h"

using namespace std;

using namespace reone::scene;

namespace reone {

namespace game {

StaticCamera::StaticCamera(SceneGraph *sceneGraph, float aspect) : _aspect(aspect) {
    _sceneNode = make_unique<CameraSceneNode>(sceneGraph, glm::mat4(1.0f));
}

void StaticCamera::setObject(const PlaceableCamera &object) {
    glm::mat4 projection(glm::perspective(glm::radians(object.fieldOfView()), _aspect, 0.1f, 10000.0f));

    _sceneNode->setLocalTransform(object.transform());
    _sceneNode->setProjection(projection);
}

} // namespace game

} // namespace reone
