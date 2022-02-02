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

#include "modelnode.h"

namespace reone {

namespace scene {

class DummySceneNode : public ModelNodeSceneNode {
public:
    DummySceneNode(
        std::shared_ptr<graphics::ModelNode> modelNode,
        SceneGraph &sceneGraph,
        graphics::GraphicsContext &graphicsContext,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::UniformBuffers &uniformBuffers) :
        ModelNodeSceneNode(
            std::move(modelNode),
            SceneNodeType::Dummy,
            sceneGraph,
            graphicsContext,
            meshes,
            shaders,
            textures,
            uniformBuffers) {
    }
};

} // namespace scene

} // namespace reone
