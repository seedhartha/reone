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

#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace reone {

namespace render {

class SceneNode {
public:
    const glm::mat4 &transform() const;
    float distanceToCamera() const;

    void setDistanceToCamera(float distance);

protected:
    SceneNode(const glm::mat4 &transform);

private:
    glm::mat4 _transform { 1.0f };
    float _distanceToCamera { 0.0f };

    SceneNode(const SceneNode &) = delete;
    SceneNode &operator=(const SceneNode &) = delete;
};

} // namespace render

} // namespace reone
