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

#include "../render/mesh/meshes.h"

#include "node/cameranode.h"
#include "node/lightnode.h"
#include "node/modelnodescenenode.h"
#include "node/modelscenenode.h"
#include "node/particlenode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static bool g_emittersEnabled = true;

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

    // Sort transparent meshes
    unordered_map<SceneNode *, float> cameraDistances;
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

    // Sort particles
    unordered_map<ParticleSceneNode *, float> particlesZ;
    for (auto &particle : _particles) {
        glm::vec4 screen(_activeCamera->projection() * _activeCamera->view() * particle->absoluteTransform()[3]);
        screen /= screen.w;
        particlesZ.insert(make_pair(particle, screen.z));
    }
    sort(_particles.begin(), _particles.end(), [&particlesZ](auto &left, auto &right) {
        float leftZ = particlesZ.find(left)->second;
        float rightZ = particlesZ.find(right)->second;
        return leftZ > rightZ;
    });
}

void SceneGraph::refreshNodeLists() {
    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _shadowMeshes.clear();
    _lights.clear();
    _particles.clear();

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
            } else if (g_emittersEnabled) {
                auto particle = dynamic_pointer_cast<ParticleSceneNode>(node);
                if (particle && _activeCamera->isInFrustum(particle->absoluteTransform()[3])) {
                    _particles.push_back(particle.get());
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

    if (!_refNode) return;

    glm::vec3 refNodePos(_refNode->absoluteTransform()[3]);
    vector<LightSceneNode *> lights;
    getLightsAt(refNodePos, lights, 1, [](auto &light) { return light.shadow(); });

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

void SceneGraph::render() const {
    if (!_activeCamera) return;

    GlobalUniforms globals;
    globals.projection = _activeCamera->projection();
    globals.view = _activeCamera->view();
    globals.cameraPosition = _activeCamera->absoluteTransform()[3];
    globals.shadowLightPresent = _shadowLightPresent;
    globals.shadowLightPosition = _shadowLightPosition;

    Shaders::instance().setGlobalUniforms(globals);

    renderNoGlobalUniforms(false);
}

void SceneGraph::renderNoGlobalUniforms(bool shadowPass) const {
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

    // Render particles
    for (auto &particle : _particles) {
        particle->renderSingle(false);
    }
}

void SceneGraph::getLightsAt(const glm::vec3 &position, vector<LightSceneNode *> &lights, int count, function<bool(const LightSceneNode &)> predicate) const {
    unordered_map<LightSceneNode *, float> distances;
    lights.clear();

    for (auto &light : _lights) {
        if (!predicate(*light)) continue;

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

    if (lights.size() > count) {
        lights.erase(lights.begin() + count, lights.end());
    }
}

void SceneGraph::setActiveCamera(const shared_ptr<CameraSceneNode> &camera) {
    _activeCamera = camera;
}

void SceneGraph::setReferenceNode(const shared_ptr<SceneNode> &node) {
    _refNode = node;
}

void SceneGraph::setUpdate(bool update) {
    _update = update;
}

void SceneGraph::setAmbientLightColor(const glm::vec3 &color) {
    _ambientLightColor = color;
}

} // namespace scene

} // namespace reone
