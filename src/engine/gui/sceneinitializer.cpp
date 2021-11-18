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

#include "sceneinitializer.h"

#include "../scene/graph.h"
#include "../scene/node/camera.h"
#include "../scene/node/mesh.h"
#include "../scene/node/model.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace gui {

void SceneInitializer::invoke() {
    shared_ptr<ModelSceneNode> model(_modelSupplier(_sceneGraph));
    if (!model) {
        throw logic_error("model is null");
    }

    glm::mat4 projection(glm::ortho(
        -_aspect * _modelScale + _modelOffset.x,
        _aspect * _modelScale + _modelOffset.x,
        -_modelScale + _modelOffset.y,
        _modelScale + _modelOffset.y,
        _zNear, _zFar));

    _sceneGraph.clear();
    _sceneGraph.addRoot(model);
    _sceneGraph.setAmbientLightColor(_ambientLightColor);

    shared_ptr<CameraSceneNode> cameraNode(_sceneGraph.newCamera("", projection));
    if (_cameraNodeName.empty()) {
        cameraNode->setLocalTransform(_cameraTransform);
    } else {
        shared_ptr<ModelNode> modelNode(model->model()->getNodeByName(_cameraNodeName));
        if (modelNode) {
            cameraNode->setLocalTransform(modelNode->absoluteTransform() * _cameraTransform);
        }
    }
    _sceneGraph.setActiveCamera(move(cameraNode));

    shared_ptr<DummySceneNode> lightingRefNode;
    if (!_lightingRefNodeName.empty()) {
        shared_ptr<ModelNode> modelNode(model->model()->getNodeByName(_lightingRefNodeName));
        if (modelNode) {
            lightingRefNode = _sceneGraph.newDummy(modelNode);
            lightingRefNode->setLocalTransform(modelNode->absoluteTransform());
        }
    }
    _sceneGraph.setLightingRefNode(move(lightingRefNode));
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

SceneInitializer &SceneInitializer::modelSupplier(const function<shared_ptr<ModelSceneNode>(SceneGraph &)> &supplier) {
    _modelSupplier = supplier;
    return *this;
}

SceneInitializer &SceneInitializer::modelScale(float scale) {
    _modelScale = scale;
    return *this;
}

SceneInitializer &SceneInitializer::modelOffset(glm::vec2 offset) {
    _modelOffset = move(offset);
    return *this;
}

SceneInitializer &SceneInitializer::cameraTransform(glm::mat4 transform) {
    _cameraTransform = move(transform);
    return *this;
}

SceneInitializer &SceneInitializer::cameraFromModelNode(string nodeName) {
    _cameraNodeName = move(nodeName);
    return *this;
}

SceneInitializer &SceneInitializer::ambientLightColor(glm::vec3 color) {
    _ambientLightColor = move(color);
    return *this;
}

SceneInitializer &SceneInitializer::lightingRefFromModelNode(string nodeName) {
    _lightingRefNodeName = move(nodeName);
    return *this;
}

} // namespace gui

} // namespace reone
