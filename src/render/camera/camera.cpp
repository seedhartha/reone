/*
 * Copyright © 2020 Vsevolod Kremianskii
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

namespace reone {

namespace render {

const glm::mat4 &Camera::projection() const {
    return _projection;
}

const glm::mat4 &Camera::view() const {
    return _view;
}

const glm::vec3 &Camera::position() const {
    return _position;
}

float Camera::heading() const {
    return _heading;
}

} // namespace render

} // namespace reone
