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
#include "node/grassnode.h"
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
    _grass.reset();
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

void SceneGraph::setGrass(shared_ptr<GrassSceneNode> node) {
    _grass = move(node);
}

void SceneGraph::prepareFrame() {
    if (!_activeCamera) return;

    refreshNodeLists();
    refreshShadowLight();

    for (auto &root : _roots) {
        if (root->type() == SceneNodeType::Model) {
            static_pointer_cast<ModelSceneNode>(root)->updateLighting();
        }
    }

    prepareOpaqueMeshes();
    prepareTransparentMeshes();
    prepareParticles();
    prepareGrass();
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
    switch (node->type()) {
        case SceneNodeType::Model: {
            // Skip the model and its children if it is not currently visible
            auto model = static_pointer_cast<ModelSceneNode>(node);
            if (!model->isVisible() || model->isCulledOut()) return;
            break;
        }
        case SceneNodeType::ModelNode: {
            // For model nodes, determine whether they should be rendered and cast shadows
            auto modelNode = static_pointer_cast<ModelNodeSceneNode>(node);
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
            break;
        }
        case SceneNodeType::Light:
            _lights.push_back(static_pointer_cast<LightSceneNode>(node).get());
            break;
        case SceneNodeType::Emitter:
            _emitters.push_back(static_pointer_cast<EmitterSceneNode>(node).get());
            break;
        default:
            break;
    }

    for (auto &child : node->children()) {
        refreshFromSceneNode(child);
    }
}

void SceneGraph::refreshShadowLight() {
    const LightSceneNode *nextShadowLight = nullptr;

    if (_shadowReference) {
        vector<LightSceneNode *> lights;
        getLightsAt(*_shadowReference, lights, 1, [](auto &light) { return light.isShadow(); });

        if (!lights.empty()) {
            nextShadowLight = lights.front();
        }
    }

    if (!nextShadowLight) {
        _shadowLight = nullptr;
        return;
    }

    if (!_shadowLight) {
        _shadowLight = nextShadowLight;
        _shadowFading = false;
        return;
    }

    if (_shadowLight == nextShadowLight) return;

    if (_shadowFading && _shadowStrength == 0.0f) {
        _shadowLight = nextShadowLight;
        _shadowFading = false;
    } else {
        _shadowFading = true;
    }
}

void SceneGraph::prepareOpaqueMeshes() {
    // Sort opaque meshes by distance to camera, so as to take advantage of early Z-test. Disabled due to performance degradation.
    /*
    glm::vec3 cameraPosition(_activeCamera->absoluteTransform()[3]);
    unordered_map<SceneNode *, float> meshToCamera;
    for (auto &mesh : _opaqueMeshes) {
        meshToCamera.insert(make_pair(mesh, mesh->getDistanceTo(cameraPosition)));
    }
    sort(_opaqueMeshes.begin(), _opaqueMeshes.end(), [&meshToCamera](auto &left, auto &right) {
        return meshToCamera.find(left)->second > meshToCamera.find(right)->second;
    });
    */
}

void SceneGraph::prepareTransparentMeshes() {
    // Sort transparent meshes by transparency and distance to camera, so as to ensure correct blending
    glm::vec3 cameraPosition(_activeCamera->absoluteTransform()[3]);
    unordered_map<SceneNode *, float> meshToCamera;
    for (auto &mesh : _transparentMeshes) {
        meshToCamera.insert(make_pair(mesh, mesh->getDistanceTo2(cameraPosition)));
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
}

void SceneGraph::prepareParticles() {
    static glm::vec4 viewport(-1.0f, -1.0f, 1.0f, 1.0f);

    // Extract particles from all emitters, sort them by depth
    vector<pair<Particle *, float>> particlesZ;
    for (auto &emitter : _emitters) {
        glm::mat4 modelView(_activeCamera->view() * emitter->absoluteTransform());
        for (auto &particle : emitter->particles()) {
            glm::vec3 screen(glm::project(particle->position(), modelView, _activeCamera->projection(), viewport));
            if (screen.z >= 0.5f && glm::abs(screen.x) <= 1.0f && glm::abs(screen.y) <= 1.0f) {
                particlesZ.push_back(make_pair(particle.get(), screen.z));
            }
        }
    }
    sort(particlesZ.begin(), particlesZ.end(), [](auto &left, auto &right) {
        return left.second > right.second;
    });

    // Map (particle, Z) pairs to (emitter, particles)
    _particles.clear();
    EmitterSceneNode *emitter = nullptr;
    vector<Particle *> emitterParticles;
    for (auto &pair : particlesZ) {
        if (pair.first->emitter() != emitter) {
            flushEmitterParticles(emitter, emitterParticles);
            emitter = pair.first->emitter();
        }
        emitterParticles.push_back(pair.first);
    }
    flushEmitterParticles(emitter, emitterParticles);
}

void SceneGraph::flushEmitterParticles(EmitterSceneNode *emitter, vector<Particle *> &particles) {
    if (emitter && !particles.empty()) {
        _particles.push_back(make_pair(emitter, particles));
        particles.clear();
    }
}

void SceneGraph::prepareGrass() {
    static glm::vec4 viewport(-1.0f, -1.0f, 1.0f, 1.0f);

    _grassClusters.clear();

    if (_grass && _activeCamera) {
        vector<pair<GrassCluster, float>> clustersZ;
        for (auto &cluster : _grass->clusters()) {
            glm::vec3 screen(glm::project(cluster.position, _activeCamera->view(), _activeCamera->projection(), viewport));
            if (screen.z >= 0.5f && glm::abs(screen.x) <= 1.0f && glm::abs(screen.y) <= 1.0f) {
                clustersZ.push_back(make_pair(cluster, screen.z));
            }
        }
        sort(clustersZ.begin(), clustersZ.end(), [](auto &left, auto &right) {
            return left.second > right.second;
        });
        int numClustersToErase = glm::max(0, static_cast<int>(clustersZ.size()) - kMaxGrassClusters);
        if (numClustersToErase > 0) {
            clustersZ.erase(clustersZ.begin(), clustersZ.begin() + numClustersToErase);
        }
        for (auto &cluster : clustersZ) {
            _grassClusters.push_back(move(cluster.first));
        }
    }
}

void SceneGraph::update(float dt) {
    if (!_update) return;

    for (auto &root : _roots) {
        root->update(dt);
    }

    if (_shadowFading) {
        _shadowStrength = glm::max(0.0f, _shadowStrength - dt);
    } else {
        _shadowStrength = glm::min(_shadowStrength + dt, 1.0f);
    }
}

void SceneGraph::draw(bool shadowPass) {
    if (!_activeCamera) return;

    if (shadowPass) {
        // Render shadow meshes
        for (auto &mesh : _shadowMeshes) {
            mesh->drawSingle(true);
        }
        return;
    }

    // Render opaque roots
    for (auto &root : _roots) {
        if (!root->isTransparent()) {
            root->draw();
        }
    }

    // Render opaque meshes
    for (auto &mesh : _opaqueMeshes) {
        mesh->drawSingle(false);
    }

    // Render transparent roots
    for (auto &root : _roots) {
        if (root->isTransparent()) {
            root->draw();
        }
    }

    // Render transparent meshes
    for (auto &mesh : _transparentMeshes) {
        mesh->drawSingle(false);
    }

    // Render grass
    if (_grass && !_grassClusters.empty()) {
        _grass->drawClusters(_grassClusters);
    }

    // Render particles
    for (auto &pair : _particles) {
        pair.first->drawParticles(pair.second);
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
        float distance = light->getDistanceTo2(node);
        if (distance > light->radius() * light->radius()) continue;

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

void SceneGraph::setUniformsPrototype(ShaderUniforms &&uniforms) {
    _uniformsPrototype = uniforms;
}

void SceneGraph::setExposure(float exposure) {
    _exposure = exposure;
}

} // namespace scene

} // namespace reone
