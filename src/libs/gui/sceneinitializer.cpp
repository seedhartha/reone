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

#include "reone/gui/sceneinitializer.h"

#include "reone/scene/graph.h"
#include "reone/scene/node/camera.h"
#include "reone/scene/node/mesh.h"
#include "reone/scene/node/model.h"

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace gui {

void SceneInitializer::invoke() {
    std::shared_ptr<ModelSceneNode> model(_modelSupplier(_sceneGraph));
    if (!model) {
        return;
    }

    _sceneGraph.clear();
    _sceneGraph.addRoot(model);
    _sceneGraph.setAmbientLightColor(_ambientLightColor);

    std::shared_ptr<CameraSceneNode> cameraNode(_sceneGraph.newCamera());
    if (!cameraNode) {
        return;
    }
    cameraNode->setOrthographicProjection(
        -_aspect * _modelScale + _modelOffset.x,
        _aspect * _modelScale + _modelOffset.x,
        -_modelScale + _modelOffset.y,
        _modelScale + _modelOffset.y,
        _zNear, _zFar);

    if (_cameraNodeName.empty()) {
        cameraNode->setLocalTransform(_cameraTransform);
    } else {
        std::shared_ptr<ModelNode> modelNode(model->model().getNodeByName(_cameraNodeName));
        if (modelNode) {
            cameraNode->setLocalTransform(modelNode->absoluteTransform() * _cameraTransform);
        }
    }
    _sceneGraph.setActiveCamera(cameraNode.get());
}

SceneInitializer &SceneInitializer::aspect(float aspect) {
    _aspect = aspect;
    return *this;
}

SceneInitializer &SceneInitializer::depth(float zNear, float zFar) {
    _zNear = zNear;
    _zFar = zFar;
    return *this;
}

SceneInitializer &SceneInitializer::modelSupplier(const std::function<std::shared_ptr<ModelSceneNode>(ISceneGraph &)> &supplier) {
    _modelSupplier = supplier;
    return *this;
}

SceneInitializer &SceneInitializer::modelScale(float scale) {
    _modelScale = scale;
    return *this;
}

SceneInitializer &SceneInitializer::modelOffset(glm::vec2 offset) {
    _modelOffset = std::move(offset);
    return *this;
}

SceneInitializer &SceneInitializer::cameraTransform(glm::mat4 transform) {
    _cameraTransform = std::move(transform);
    return *this;
}

SceneInitializer &SceneInitializer::cameraFromModelNode(std::string nodeName) {
    _cameraNodeName = std::move(nodeName);
    return *this;
}

SceneInitializer &SceneInitializer::ambientLightColor(glm::vec3 color) {
    _ambientLightColor = std::move(color);
    return *this;
}

} // namespace gui

} // namespace reone
