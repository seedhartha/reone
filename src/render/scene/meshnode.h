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

#include "../model/modelnode.h"
#include "../modelinstance.h"

#include "scenenode.h"

namespace reone {

namespace render {

class MeshSceneNode : public SceneNode {
public:
    MeshSceneNode(const ModelInstance *model, const ModelNode *modelNode, const glm::mat4 &transform);

    bool isTransparent() const;

    const ModelInstance *model() const;
    const ModelNode *modelNode() const;
    const glm::vec3 &origin() const;

private:
    const ModelInstance *_model { nullptr };
    const ModelNode *_modelNode { nullptr };
    glm::vec3 _origin { 0.0f };
};

} // namespace render

} // namespace reone
