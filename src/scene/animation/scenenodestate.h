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

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "../types.h"

namespace reone {

namespace scene {

/**
 * @see SceneNodeStateFlags
 */
struct SceneNodeState {
    int flags { 0 };
    glm::mat4 transform { 1.0f };
    float alpha { 1.0f };
    glm::vec3 selfIllumColor { 0.0f };
    glm::vec3 lightColor { 1.0f };
    float lightMultiplier { 1.0f };
    float lightRadius { 1.0f };
};

} // namespace scene

} // namespace reone
