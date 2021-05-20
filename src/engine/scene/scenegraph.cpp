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
#include <stdexcept>

#include "glm/gtx/transform.hpp"

#include "../common/guardutil.h"
#include "../graphics/mesh/meshes.h"
#include "../graphics/stateutil.h"

#include "node/cameranode.h"
#include "node/emitternode.h"
#include "node/grassnode.h"
#include "node/lightnode.h"
#include "node/meshnode.h"
#include "node/modelnode.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kMaxGrassDistance = 16.0f;

static const bool g_debugAABB = false;

SceneGraph::SceneGraph(GraphicsOptions opts, Shaders *shaders, Meshes *meshes, Textures *textures, Materials *materials, PBRIBL *pbrIbl) :
    _opts(move(opts)),
    _shaders(shaders),
    _meshes(meshes),
    _textures(textures),
    _materials(materials),
    _pbrIbl(pbrIbl) {

    ensureNotNull(shaders, "shaders");
    ensureNotNull(meshes, "meshes");
    ensureNotNull(textures, "textures");
    ensureNotNull(materials, "materials");
    ensureNotNull(pbrIbl, "pbrIbl");
}

void SceneGraph::clearRoots() {
    _roots.clear();
}

void SceneGraph::addRoot(shared_ptr<SceneNode> node) {
    _roots.push_back(move(node));
}

void SceneGraph::removeRoot(const shared_ptr<SceneNode> &node) {
    auto maybeRoot = find(_roots.begin(), _roots.end(), node);
    if (maybeRoot != _roots.end()) {
        _roots.erase(maybeRoot);
    }
}

void SceneGraph::update(float dt) {
    if (_updateRoots) {
        for (auto &root : _roots) {
            root->update(dt);
        }
    }
    if (_activeCamera) {
        cullRoots();
        refreshNodeLists();
        refreshShadowLight();
        updateLighting();
        updateShadows(dt);
        prepareTransparentMeshes();
        prepareLeafs();
    }
}

void SceneGraph::cullRoots() {
    for (auto &root : _roots) {
        if (root->type() != SceneNodeType::Model) continue;

        auto modelRoot = static_pointer_cast<ModelSceneNode>(root);

        bool culled =
            !modelRoot->isVisible() ||
            modelRoot->getDistanceTo2(*_activeCamera) > modelRoot->drawDistance() * modelRoot->drawDistance() ||
            (modelRoot->isCullable() && !_activeCamera->isInFrustum(*root));

        modelRoot->setCulled(culled);
    }
}

void SceneGraph::updateLighting() {
    _closestLights.clear();

    if (_lightingRefNode) {
        getLightsAt(*_lightingRefNode, _closestLights, kMaxLights, [](auto &light) { return !light.modelNode()->light()->ambientOnly; });
    }
}

void SceneGraph::updateShadows(float dt) {
    // Gradually fade the shadow in and out
    if (_shadowFading) {
        _shadowStrength = glm::max(0.0f, _shadowStrength - dt);
    } else {
        _shadowStrength = glm::min(_shadowStrength + dt, 1.0f);
    }
}

void SceneGraph::refreshNodeLists() {
    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _shadowMeshes.clear();
    _lights.clear();
    _emitters.clear();
    _grass.clear();

    for (auto &root : _roots) {
        refreshFromSceneNode(root);
    }
}

void SceneGraph::refreshFromSceneNode(const std::shared_ptr<SceneNode> &node) {
    bool propagate = true;

    switch (node->type()) {
        case SceneNodeType::Model: {
            // Ignore models that have been culled
            auto model = static_pointer_cast<ModelSceneNode>(node);
            if (model->isCulled()) {
                propagate = false;
            }
            break;
        }
        case SceneNodeType::Mesh: {
            // For model nodes, determine whether they should be rendered and cast shadows
            auto modelNode = static_pointer_cast<MeshSceneNode>(node);
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
        case SceneNodeType::Grass:
            _grass.push_back(static_pointer_cast<GrassSceneNode>(node).get());
            break;
        default:
            break;
    }

    if (propagate) {
        for (auto &child : node->children()) {
            refreshFromSceneNode(child);
        }
    }
}

void SceneGraph::refreshShadowLight() {
    const LightSceneNode *nextShadowLight = nullptr;

    if (_lightingRefNode) {
        vector<LightSceneNode *> lights;
        getLightsAt(*_lightingRefNode, lights, 1, [](auto &light) { return light.modelNode()->light()->shadow; });

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

void SceneGraph::prepareTransparentMeshes() {
    // Sort transparent meshes by transparency and distance to camera, so as to ensure correct blending
    glm::vec3 cameraPosition(_activeCamera->absoluteTransform()[3]);
    unordered_map<SceneNode *, float> meshToCamera;
    for (auto &mesh : _transparentMeshes) {
        meshToCamera.insert(make_pair(mesh, mesh->getDistanceTo2(cameraPosition)));
    }
    sort(_transparentMeshes.begin(), _transparentMeshes.end(), [&meshToCamera](auto &left, auto &right) {
        int leftTransparency = left->modelNode()->mesh()->transparency;
        int rightTransparency = right->modelNode()->mesh()->transparency;

        if (leftTransparency < rightTransparency) return true;
        if (leftTransparency > rightTransparency) return false;

        float leftDistance = meshToCamera.find(left)->second;
        float rightDistance = meshToCamera.find(right)->second;

        return leftDistance > rightDistance;
    });
}

void SceneGraph::prepareLeafs() {
    static glm::vec4 viewport(-1.0f, -1.0f, 1.0f, 1.0f);

    vector<pair<shared_ptr<SceneNodeElement>, float>> elements;
    glm::vec3 cameraPos(_activeCamera->absoluteTransform()[3]);

    // Add grass clusters
    for (auto &grass : _grass) {
        float grassDistance2 = kMaxGrassDistance * kMaxGrassDistance;
        size_t numLeafs = elements.size();
        for (auto &cluster : grass->clusters()) {
            float distance2 = glm::distance2(cameraPos, cluster->position);
            if (distance2 <= grassDistance2) {
                glm::vec3 screen(glm::project(cluster->position, _activeCamera->view(), _activeCamera->projection(), viewport));
                if (screen.z >= 0.5f && glm::abs(screen.x) <= 1.0f && glm::abs(screen.y) <= 1.0f) {
                    elements.push_back(make_pair(cluster, screen.z));
                }
            }
        }
    }

    // Add particles
    for (auto &emitter : _emitters) {
        glm::mat4 modelView(_activeCamera->view() * emitter->absoluteTransform());
        for (auto &particle : emitter->particles()) {
            glm::vec3 screen(glm::project(particle->position, modelView, _activeCamera->projection(), viewport));
            if (screen.z >= 0.5f && glm::abs(screen.x) <= 1.0f && glm::abs(screen.y) <= 1.0f) {
                elements.push_back(make_pair(particle, screen.z));
            }
        }
    }

    // Sort elements back to front
    sort(elements.begin(), elements.end(), [](auto &left, auto &right) { return left.second > right.second; });

    // Group elements into buckets
    _elements.clear();
    vector<shared_ptr<SceneNodeElement>> nodeElements;
    for (auto &elementDepth : elements) {
        if (!nodeElements.empty()) {
            _elements.push_back(make_pair(nodeElements[0]->parent, nodeElements));
            nodeElements.clear();
        }
        nodeElements.push_back(elementDepth.first);
    }
    if (!nodeElements.empty()) {
        _elements.push_back(make_pair(nodeElements[0]->parent, nodeElements));
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

    setBackFaceCullingEnabled(true);

    // Render opaque meshes
    for (auto &mesh : _opaqueMeshes) {
        mesh->drawSingle(false);
    }

    if (g_debugAABB) {
        for (auto &root : _roots) {
            glm::mat4 transform(root->absoluteTransform());
            transform *= glm::scale(root->aabb().getSize());

            ShaderUniforms uniforms(_uniformsPrototype);
            uniforms.combined.general.model = move(transform);

            _shaders->activate(ShaderProgram::SimpleColor, uniforms);
            _meshes->getAABB()->draw();
        }
    }

    // Render transparent meshes
    for (auto &mesh : _transparentMeshes) {
        mesh->drawSingle(false);
    }

    setBackFaceCullingEnabled(false);

    // Render particles and grass clusters
    for (auto &nodeLeaf : _elements) {
        int count = nodeLeaf.first->type() == SceneNodeType::Grass && nodeLeaf.second.size() > kMaxGrassClusters ?
            kMaxGrassClusters :
            -1;
        nodeLeaf.first->drawElements(nodeLeaf.second, count);
    }

    // Render lens flares
    for (auto &light : _lights) {
        // Ignore lights that are too far away or outside of camera frustum
        float flareRadius = light->modelNode()->light()->flareRadius;
        if (_activeCamera->getDistanceTo2(*light) > flareRadius * flareRadius ||
            !_activeCamera->isInFrustum(light->absoluteTransform()[3])) continue;

        for (auto &flare : light->modelNode()->light()->flares) {
            light->drawLensFlares(flare);
        }
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
        int leftPriority = left->modelNode()->light()->priority;
        int rightPriority = right->modelNode()->light()->priority;

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

} // namespace scene

} // namespace reone
