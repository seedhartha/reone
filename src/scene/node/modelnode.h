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

#include "../node.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class Meshes;
class ModelNode;
class Shaders;

} // namespace graphics

namespace scene {

class ModelNodeSceneNode : public SceneNode {
public:
    const graphics::ModelNode &modelNode() const { return *_modelNode; }

protected:
    std::shared_ptr<graphics::ModelNode> _modelNode;

    ModelNodeSceneNode(
        std::shared_ptr<graphics::ModelNode> modelNode,
        SceneNodeType type,
        SceneGraph &sceneGraph,
        graphics::GraphicsContext &graphicsContext,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders) :
        SceneNode(type, sceneGraph),
        _modelNode(std::move(modelNode)),
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders) {
    }

protected:
    // Services

    graphics::GraphicsContext &_graphicsContext;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;

    // END Services
};

} // namespace scene

} // namespace reone
