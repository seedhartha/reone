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

#include "scenegraph.h"

#include <algorithm>

#include "../render/meshes.h"

#include "node/cameranode.h"
#include "node/emitternode.h"
#include "node/lightnode.h"
#include "node/modelnodescenenode.h"
#include "node/modelscenenode.h"

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
    auto maybeRoot = find(_roots.begin(), _roots.end(), node);
    if (maybeRoot != _roots.end()) {
        _roots.erase(maybeRoot);
    }
}

void SceneGraph::prepareFrame() {
    if (!_activeCamera) return;

    refreshNodeLists();
    refreshShadowLight();

    for (auto &root : _roots) {
        ModelSceneNode *modelNode = dynamic_cast<ModelSceneNode *>(root.get());
        if (modelNode) {
            modelNode->updateLighting();
        }
    }

    glm::vec3 cameraPosition(_activeCamera->absoluteTransform()[3]);

    // Sort transparent meshes by transparency and distance to camera
    unordered_map<SceneNode *, float> meshToCamera;
    for (auto &mesh : _transparentMeshes) {
        meshToCamera.insert(make_pair(mesh, mesh->getDistanceTo(cameraPosition)));
    }
    sort(_transparentMeshes.begin(), _transparentMeshes.end(), [&meshToCamera](auto &left, auto &right) {
        int leftTransparency = left->modelNode()->mesh()->transparency();
        int rightTransparency = right->modelNode()->mesh()->transparency();

        if (leftTransparency < rightTransparency) return true;
        if (leftTransparency > rightTransparency) return false;

        float leftDistance = meshToCamera.find(left)->second;
        float rightDistance = meshToCamera.find(right)->second;

        return leftDistance > rightDistance;
    });

    // Sort emitters by render order and distance to camera
    unordered_map<SceneNode *, float> emitterToCamera;
    for (auto &emitter : _emitters) {
        emitterToCamera.insert(make_pair(emitter, emitter->getDistanceTo(cameraPosition)));
    }
    sort(_emitters.begin(), _emitters.end(), [&emitterToCamera](auto &left, auto &right) {
        int leftRenderOrder = left->emitter()->renderOrder();
        int rightRenderOrder = right->emitter()->renderOrder();

        if (leftRenderOrder > rightRenderOrder) return true;
        if (leftRenderOrder < rightRenderOrder) return false;

        float leftDistance = emitterToCamera.find(left)->second;
        float rightDistance = emitterToCamera.find(right)->second;

        return leftDistance > rightDistance;
    });
}

void SceneGraph::refreshNodeLists() {
    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _shadowMeshes.clear();
    _lights.clear();
    _emitters.clear();

    for (auto &root : _roots) {
        refreshFromSceneNode(root);
    }
}

void SceneGraph::refreshFromSceneNode(const std::shared_ptr<SceneNode> &node) {
    auto model = dynamic_pointer_cast<ModelSceneNode>(node);
    if (model) {
        // Skip the model and its children if it is not currently visible
        if (!model->isVisible() || !model->isOnScreen()) return;

    } else {
        auto modelNode = dynamic_pointer_cast<ModelNodeSceneNode>(node);
        if (modelNode) {
            // For model nodes, determine whether they should be rendered and cast shadows
            if (modelNode->shouldRender()) {
                // Sort model nodes into transparent and opaque
                if (modelNode->isTransparent()) {
                    _transparentMeshes.push_back(modelNode.get());
                } else {
                    _opaqueMeshes.push_back(modelNode.get());
                }
            }
            if (modelNode->shouldCastShadows()) {
                _shadowMeshes.push_back(modelNode.get());
            }
        } else {
            auto light = dynamic_pointer_cast<LightSceneNode>(node);
            if (light) {
                _lights.push_back(light.get());
            } else {
                auto emitter = dynamic_pointer_cast<EmitterSceneNode>(node);
                if (emitter) {
                    _emitters.push_back(emitter.get());
                }
            }
        }
    }

    for (auto &child : node->children()) {
        refreshFromSceneNode(child);
    }
}

void SceneGraph::refreshShadowLight() {
    _shadowLightPresent = false;

    if (!_shadowReference) return;

    vector<LightSceneNode *> lights;
    getLightsAt(*_shadowReference, lights, 1, [](auto &light) { return light.isShadow(); });

    if (!lights.empty()) {
        _shadowLightPresent = true;
        _shadowLightPosition = lights.front()->absoluteTransform()[3];
    }
}

void SceneGraph::update(float dt) {
    if (!_update) return;

    for (auto &root : _roots) {
        root->update(dt);
    }
}

void SceneGraph::render(bool shadowPass) {
    if (!_activeCamera) return;

    if (shadowPass) {
        // Render shadow meshes
        for (auto &mesh : _shadowMeshes) {
            mesh->renderSingle(true);
        }
        return;
    }

    // Render opaque roots
    for (auto &root : _roots) {
        if (!root->isTransparent()) {
            root->render();
        }
    }

    // Render opaque meshes
    for (auto &mesh : _opaqueMeshes) {
        mesh->renderSingle(false);
    }

    // Render transparent roots
    for (auto &root : _roots) {
        if (root->isTransparent()) {
            root->render();
        }
    }

    // Render transparent meshes
    for (auto &mesh : _transparentMeshes) {
        mesh->renderSingle(false);
    }

    // Render emitter particles
    for (auto &emitter : _emitters) {
        emitter->renderSingle(false);
    }
}

void SceneGraph::getLightsAt(
    const SceneNode &node,
    vector<LightSceneNode *> &lights,
    int count,
    function<bool(const LightSceneNode &)> predicate) const {

    unordered_map<LightSceneNode *, float> distances;
    lights.clear();

    for (auto &light : _lights) {
        if (!predicate(*light)) continue;

        // Only account for lights whose distance to the reference node is
        // within range of the light.
        float distance = light->getDistanceTo(node);
        if (distance > light->radius()) continue;

        lights.push_back(light);
        distances.insert(make_pair(light, distance));
    }

    // Sort lights by priority and radius
    sort(lights.begin(), lights.end(), [&distances](LightSceneNode *left, LightSceneNode *right) {
        if (left->priority() < right->priority()) return true;
        if (left->priority() > right->priority()) return false;

        float leftDistance = distances.find(left)->second;
        float rightDistance = distances.find(right)->second;

        return leftDistance < rightDistance;
    });

    if (lights.size() > count) {
        lights.erase(lights.begin() + count, lights.end());
    }
}

void SceneGraph::setActiveCamera(const shared_ptr<CameraSceneNode> &camera) {
    _activeCamera = camera;
}

void SceneGraph::setShadowReference(const shared_ptr<SceneNode> &reference) {
    _shadowReference = reference;
}

void SceneGraph::setUpdate(bool update) {
    _update = update;
}

void SceneGraph::setAmbientLightColor(const glm::vec3 &color) {
    _ambientLightColor = color;
}

void SceneGraph::setUniformsPrototype(ShaderUniforms uniforms) {
    _uniformsPrototype = move(uniforms);
}

void SceneGraph::setExposure(float exposure) {
    _exposure = exposure;
}

} // namespace scene

} // namespace reone
