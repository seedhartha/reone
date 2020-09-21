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

#include "glm/vec3.hpp"

#include "../model/modelnode.h"
#include "../shaders.h"

#include "scenenode.h"

namespace reone {

namespace render {

class ModelSceneNode;

class MeshSceneNode : public SceneNode {
public:
    MeshSceneNode(const ModelSceneNode *model, const ModelNode *modelNode);

    void fillSceneGraph() override;
    void updateDistanceToCamera(const glm::vec3 &cameraPosition);
    void render() const override;

    const ModelSceneNode *model() const;
    const ModelNode *modelNode() const;
    float distanceToCamera() const;

private:
    const ModelSceneNode *_model { nullptr };
    const ModelNode *_modelNode { nullptr };
    glm::vec3 _center { 0.0f };
    float _distanceToCamera { 0.0f };

    bool isTransparent() const;

    ShaderProgram getShaderProgram(const ModelMesh &mesh, bool skeletal) const;
    void updateAbsoluteTransform() override;
    void updateCenter();
};

} // namespace render

} // namespace reone
