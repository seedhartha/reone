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

#include <vector>

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "types.h"

namespace reone {

namespace render {

class ModelInstance;
class ModelNode;

struct RenderListItem {
    const ModelInstance *model { nullptr };
    const ModelNode *node { nullptr };
    glm::mat4 transform { 1.0f };
    glm::vec3 origin { 0.0f };
};

class RenderList : public std::vector<RenderListItem> {
public:
    void sortByDistanceToCamera(const glm::vec3 &cameraPosition);
    void render(bool debug) const;
};

} // namespace render

} // namespace reone
