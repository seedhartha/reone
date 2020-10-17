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

#include "scenegraph.h"

#include <algorithm>

#include "GL/glew.h"

#include "../mesh/quad.h"

#include "modelnodescenenode.h"
#include "modelscenenode.h"

using namespace std;

namespace reone {

namespace render {

static const int kMaxLightCount = 8;

SceneGraph::SceneGraph(const GraphicsOptions &opts) : _opts(opts) {
}

void SceneGraph::clear() {
    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _lights.clear();
    _rootNodes.clear();
}

void SceneGraph::addRoot(const shared_ptr<SceneNode> &node) {
    _rootNodes.push_back(node);
}

void SceneGraph::addOpaqueMesh(ModelNodeSceneNode *node) {
    _opaqueMeshes.push_back(node);
}

void SceneGraph::addTransparentMesh(ModelNodeSceneNode *node) {
    _transparentMeshes.push_back(node);
}

void SceneGraph::addLight(ModelNodeSceneNode *node) {
    _lights.push_back(node);
}

void SceneGraph::prepare() {
    if (!_activeCamera) return;

    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _lights.clear();

    for (auto &node : _rootNodes) {
        node->fillSceneGraph();
    }
    for (auto &node : _rootNodes) {
        ModelSceneNode *modelNode = dynamic_cast<ModelSceneNode *>(node.get());
        if (modelNode) {
            modelNode->updateLighting();
        }
    }

    unordered_map<ModelNodeSceneNode *, float> cameraDistances;
    glm::vec3 cameraPosition(_activeCamera->absoluteTransform()[3]);

    for (auto &mesh : _transparentMeshes) {
        cameraDistances.insert(make_pair(mesh, mesh->distanceTo(cameraPosition)));
    }
    sort(_transparentMeshes.begin(), _transparentMeshes.end(), [&cameraDistances](ModelNodeSceneNode *left, ModelNodeSceneNode *right) {
        float leftDistance = cameraDistances.find(left)->second;
        float rightDistance = cameraDistances.find(right)->second;
        return leftDistance > rightDistance;
    });
}

void SceneGraph::render() const {
    if (!_activeCamera) return;

    GlobalUniforms globals;
    globals.projection = _activeCamera->projection();
    globals.view = _activeCamera->view();
    globals.cameraPosition = _activeCamera->absoluteTransform()[3];

    Shaders.setGlobalUniforms(globals);

    for (auto &node : _rootNodes) {
        node->render();
    }
    for (auto &mesh : _opaqueMeshes) {
        mesh->renderSingle();
    }
    for (auto &mesh : _transparentMeshes) {
        mesh->renderSingle();
    }
}

void SceneGraph::getLightsAt(const glm::vec3 &position, vector<ModelNodeSceneNode *> &lights) const {
    unordered_map<ModelNodeSceneNode *, float> distances;
    lights.clear();

    for (auto &light : _lights) {
        if (light->modelNode()->light()->ambientOnly) continue;

        float distance = light->distanceTo(position);
        const ModelNode *modelNode = light->modelNode();
        float radius = modelNode->radius();

        if (distance > radius * radius) continue;

        distances.insert(make_pair(light, distance));
        lights.push_back(light);
    }

    sort(lights.begin(), lights.end(), [&distances](ModelNodeSceneNode *left, ModelNodeSceneNode *right) {
        int leftPriority = left->modelNode()->light()->priority;
        int rightPriority = right->modelNode()->light()->priority;

        if (leftPriority < rightPriority) return true;
        if (leftPriority > rightPriority) return false;

        float leftDistance = distances.find(left)->second;
        float rightDistance = distances.find(right)->second;

        return leftDistance < rightDistance;
    });

    if (lights.size() > kMaxLightCount) {
        lights.erase(lights.begin() + kMaxLightCount, lights.end());
    }
}

const glm::vec3 &SceneGraph::ambientLightColor() const {
    return _ambientLightColor;
}

void SceneGraph::setActiveCamera(const shared_ptr<CameraSceneNode> &camera) {
    _activeCamera = camera;
}

void SceneGraph::setAmbientLightColor(const glm::vec3 &color) {
    _ambientLightColor = color;
}

} // namespace render

} // namespace reone
