/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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
#include <stack>

#include "../render/mesh/quad.h"

#include "cameranode.h"
#include "lightnode.h"
#include "modelnodescenenode.h"
#include "modelscenenode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

SceneGraph::SceneGraph(const GraphicsOptions &opts) : _opts(opts) {
}

void SceneGraph::clear() {
    _roots.clear();
}

void SceneGraph::addRoot(const shared_ptr<SceneNode> &node) {
    _roots.push_back(node);
}

void SceneGraph::removeRoot(const shared_ptr<SceneNode> &node) {
    auto maybeRoot = find_if(_roots.begin(), _roots.end(), [&node](auto &n) { return n.get() == node.get(); });
    if (maybeRoot != _roots.end()) {
        _roots.erase(maybeRoot);
    }
}

void SceneGraph::build() {
}

void SceneGraph::prepareFrame() {
    if (!_activeCamera) return;

    refreshMeshesAndLights();

    for (auto &root : _roots) {
        ModelSceneNode *modelNode = dynamic_cast<ModelSceneNode *>(root.get());
        if (modelNode) {
            modelNode->updateLighting();
        }
    }
    unordered_map<ModelNodeSceneNode *, float> cameraDistances;
    glm::vec3 cameraPosition(_activeCamera->absoluteTransform()[3]);

    for (auto &mesh : _transparentMeshes) {
        cameraDistances.insert(make_pair(mesh, mesh->distanceTo(cameraPosition)));
    }
    sort(_transparentMeshes.begin(), _transparentMeshes.end(), [&cameraDistances](auto &left, auto &right) {
        int leftTransparency = left->modelNode()->mesh()->transparency();
        int rightTransparency = right->modelNode()->mesh()->transparency();

        if (leftTransparency < rightTransparency) return true;
        if (leftTransparency > rightTransparency) return false;

        float leftDistance = cameraDistances.find(left)->second;
        float rightDistance = cameraDistances.find(right)->second;

        return leftDistance > rightDistance;
    });
}

void SceneGraph::refreshMeshesAndLights() {
    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _lights.clear();

    for (auto &root : _roots) {
        stack<SceneNode *> nodes;
        nodes.push(root.get());

        while (!nodes.empty()) {
            SceneNode *node = nodes.top();
            nodes.pop();

            ModelSceneNode *model = dynamic_cast<ModelSceneNode *>(node);
            if (model) {
                if (!model->isVisible() || !model->isOnScreen()) continue;

            } else {
                ModelNodeSceneNode *modelNode = dynamic_cast<ModelNodeSceneNode *>(node);
                if (modelNode) {
                    if (modelNode->shouldRender()) {
                        if (modelNode->isTransparent()) {
                            _transparentMeshes.push_back(modelNode);
                        } else {
                            _opaqueMeshes.push_back(modelNode);
                        }
                    }
                } else {
                    LightSceneNode *light = dynamic_cast<LightSceneNode *>(node);
                    if (light) {
                        _lights.push_back(light);
                    }
                }
            }
            for (auto &child : node->children()) {
                nodes.push(child.get());
            }
        }
    }
}

void SceneGraph::render() const {
    if (!_activeCamera) return;

    GlobalUniforms globals;
    globals.projection = _activeCamera->projection();
    globals.view = _activeCamera->view();
    globals.cameraPosition = _activeCamera->absoluteTransform()[3];

    Shaders::instance().setGlobalUniforms(globals);

    for (auto &root : _roots) {
        root->render();
    }
    for (auto &mesh : _opaqueMeshes) {
        mesh->renderSingle();
    }
    for (auto &mesh : _transparentMeshes) {
        mesh->renderSingle();
    }
}

void SceneGraph::getLightsAt(const glm::vec3 &position, vector<LightSceneNode *> &lights) const {
    unordered_map<LightSceneNode *, float> distances;
    lights.clear();

    for (auto &light : _lights) {
        float distance = light->distanceTo(position);
        float radius = light->radius();
        if (distance > radius) continue;

        distances.insert(make_pair(light, distance));
        lights.push_back(light);
    }

    sort(lights.begin(), lights.end(), [&distances](LightSceneNode *left, LightSceneNode *right) {
        int leftPriority = left->priority();
        int rightPriority = right->priority();

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

} // namespace scene

} // namespace reone
