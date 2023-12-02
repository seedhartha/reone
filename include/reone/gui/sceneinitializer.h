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

#pragma once

#include "reone/graphics/options.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class MeshRegistry;
class ShaderRegistry;
class Textures;

} // namespace graphics

namespace scene {

class ModelSceneNode;
class ISceneGraph;

} // namespace scene

namespace gui {

class SceneInitializer {
public:
    SceneInitializer(scene::ISceneGraph &sceneGraph) :
        _sceneGraph(sceneGraph) {
    }

    void invoke();

    SceneInitializer &aspect(float aspect);
    SceneInitializer &depth(float zNear, float zFar);
    SceneInitializer &modelSupplier(const std::function<std::shared_ptr<scene::ModelSceneNode>(scene::ISceneGraph &)> &supplier);
    SceneInitializer &modelScale(float scale);
    SceneInitializer &modelOffset(glm::vec2 offset);
    SceneInitializer &cameraTransform(glm::mat4 transform);
    SceneInitializer &cameraFromModelNode(std::string nodeName);
    SceneInitializer &ambientLightColor(glm::vec3 color);

private:
    scene::ISceneGraph &_sceneGraph;

    float _aspect {1.0f};
    float _zNear {graphics::kDefaultClipPlaneNear};
    float _zFar {graphics::kDefaultClipPlaneFar};
    std::function<std::shared_ptr<scene::ModelSceneNode>(scene::ISceneGraph &)> _modelSupplier;
    float _modelScale {1.0f};
    glm::vec2 _modelOffset {0.0f};
    glm::mat4 _cameraTransform {1.0f};
    std::string _cameraNodeName;
    glm::vec3 _ambientLightColor {0.0f};
};

} // namespace gui

} // namespace reone
