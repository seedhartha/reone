/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "camera.h"

using namespace std;

using namespace reone::scene;

namespace reone {

namespace game {

bool Camera::handle(const SDL_Event &event) {
    return false;
}

void Camera::update(float dt) {
}

void Camera::stopMovement() {
}

float Camera::heading() const {
    return _heading;
}

shared_ptr<CameraSceneNode> Camera::sceneNode() const {
    return _sceneNode;
}

} // namespace game

} // namespace reone
