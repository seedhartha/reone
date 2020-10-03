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

#include <memory>
#include <vector>

#include "glm/vec3.hpp"

#include "cameranode.h"
#include "lightnode.h"
#include "meshnode.h"

namespace reone {

namespace render {

class SceneGraph {
public:
    SceneGraph(const GraphicsOptions &opts);

    void clear();
    void addRoot(const std::shared_ptr<SceneNode> &node);
    void addOpaqueMesh(MeshSceneNode *node);
    void addTransparentMesh(MeshSceneNode *node);
    void addLight(LightSceneNode *node);
    void prepare();
    void render() const;

    void getLightsAt(const glm::vec3 &position, std::vector<LightSceneNode *> &lights) const;

    const glm::vec3 &ambientLightColor() const;

    void setActiveCamera(const std::shared_ptr<CameraSceneNode> &camera);
    void setAmbientLightColor(const glm::vec3 &color);

private:
    GraphicsOptions _opts;
    std::vector<std::shared_ptr<SceneNode>> _rootNodes;
    std::vector<MeshSceneNode *> _opaqueMeshes;
    std::vector<MeshSceneNode *> _transparentMeshes;
    std::vector<LightSceneNode *> _lights;
    std::shared_ptr<CameraSceneNode> _activeCamera;
    glm::vec3 _ambientLightColor { 0.5f };
    uint32_t _textureId { 0 };

    SceneGraph(const SceneGraph &) = delete;
    SceneGraph &operator=(const SceneGraph &) = delete;
};

} // namespace render

} // namespace reone
