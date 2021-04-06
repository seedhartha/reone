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

#include <functional>
#include <memory>

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "../scene/node/modelscenenode.h"
#include "../scene/scenegraph.h"

namespace reone {

namespace gui {

/**
 * Utility class for initialization of 3D controls.
 */
class SceneBuilder {
public:
    SceneBuilder(const render::GraphicsOptions &opts);

    std::unique_ptr<scene::SceneGraph> build();

    SceneBuilder &aspect(float aspect);
    SceneBuilder &depth(float zNear, float zFar);
    SceneBuilder &modelSupplier(const std::function<std::shared_ptr<scene::ModelSceneNode>(scene::SceneGraph &)> &supplier);
    SceneBuilder &modelScale(float scale);
    SceneBuilder &cameraTransform(const glm::mat4 &transform);
    SceneBuilder &cameraFromModelNode(const std::string &nodeName);
    SceneBuilder &ambientLightColor(const glm::vec3 &color);

private:
    render::GraphicsOptions _opts;

    float _aspect { 1.0f };
    float _zNear { 0.0f };
    float _zFar { 0.0f };
    std::function<std::shared_ptr<scene::ModelSceneNode>(scene::SceneGraph &)> _modelSupplier;
    float _modelScale { 1.0f };
    glm::vec2 _modelOffset { 0.0f };
    glm::mat4 _cameraTransform { 1.0f };
    std::string _cameraNodeName;
    glm::vec3 _ambientLightColor { 0.0f };
};

} // namespace gui

} // namespace reone
