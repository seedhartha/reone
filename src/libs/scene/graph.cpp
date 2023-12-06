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

#include "reone/scene/graph.h"

#include "reone/audio/di/services.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/walkmesh.h"
#include "reone/scene/collision.h"
#include "reone/scene/node/camera.h"
#include "reone/scene/node/emitter.h"
#include "reone/scene/node/grass.h"
#include "reone/scene/node/grasscluster.h"
#include "reone/scene/node/light.h"
#include "reone/scene/node/mesh.h"
#include "reone/scene/node/model.h"
#include "reone/scene/node/particle.h"
#include "reone/scene/node/sound.h"
#include "reone/scene/node/trigger.h"
#include "reone/scene/node/walkmesh.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr int kMaxFlareLights = 4;
static constexpr int kMaxSoundCount = 4;

static constexpr float kShadowFadeSpeed = 2.0f;
static constexpr float kElevationTestZ = 1024.0f;

static constexpr float kLightRadiusBias = 64.0f;
static constexpr float kLightRadiusBias2 = kLightRadiusBias * kLightRadiusBias;

static constexpr float kMaxCollisionDistanceWalk = 8.0f;
static constexpr float kMaxCollisionDistanceWalk2 = kMaxCollisionDistanceWalk * kMaxCollisionDistanceWalk;

static constexpr float kMaxCollisionDistanceLineOfSight = 16.0f;
static constexpr float kMaxCollisionDistanceLineOfSight2 = kMaxCollisionDistanceLineOfSight * kMaxCollisionDistanceLineOfSight;

void SceneGraph::clear() {
    _modelRoots.clear();
    _walkmeshRoots.clear();
    _soundRoots.clear();
    _grassRoots.clear();
    _activeLights.clear();
}

void SceneGraph::addRoot(std::shared_ptr<ModelSceneNode> node) {
    _modelRoots.push_back(node);
}

void SceneGraph::addRoot(std::shared_ptr<WalkmeshSceneNode> node) {
    if (node->walkmesh().isAreaWalkmesh()) {
        _walkmeshRoots.push_back(node);
    } else {
        _walkmeshRoots.push_front(node);
    }
}

void SceneGraph::addRoot(std::shared_ptr<TriggerSceneNode> node) {
    _triggerRoots.push_back(node);
}

void SceneGraph::addRoot(std::shared_ptr<GrassSceneNode> node) {
    _grassRoots.push_back(node);
}

void SceneGraph::addRoot(std::shared_ptr<SoundSceneNode> node) {
    _soundRoots.push_back(node);
}

void SceneGraph::removeRoot(ModelSceneNode &node) {
    for (auto it = _activeLights.begin(); it != _activeLights.end();) {
        if (&(*it)->model() == &node) {
            it = _activeLights.erase(it);
        } else {
            ++it;
        }
    }

    auto it = std::remove_if(
        _modelRoots.begin(),
        _modelRoots.end(),
        [&node](auto &root) { return root.get() == &node; });
    _modelRoots.erase(it, _modelRoots.end());
}

void SceneGraph::removeRoot(WalkmeshSceneNode &node) {
    auto it = std::remove_if(
        _walkmeshRoots.begin(),
        _walkmeshRoots.end(),
        [&node](auto &root) { return root.get() == &node; });
    _walkmeshRoots.erase(it, _walkmeshRoots.end());
}

void SceneGraph::removeRoot(TriggerSceneNode &node) {
    auto it = std::remove_if(
        _triggerRoots.begin(),
        _triggerRoots.end(),
        [&node](auto &root) { return root.get() == &node; });
    _triggerRoots.erase(it, _triggerRoots.end());
}

void SceneGraph::removeRoot(GrassSceneNode &node) {
    auto it = std::remove_if(
        _grassRoots.begin(),
        _grassRoots.end(),
        [&node](auto &root) { return root.get() == &node; });
    _grassRoots.erase(it, _grassRoots.end());
}

void SceneGraph::removeRoot(SoundSceneNode &node) {
    auto it = std::remove_if(
        _soundRoots.begin(),
        _soundRoots.end(),
        [&node](auto &root) { return root.get() == &node; });
    _soundRoots.erase(it, _soundRoots.end());
}

void SceneGraph::update(float dt) {
    if (_updateRoots) {
        for (auto &root : _modelRoots) {
            root->update(dt);
        }
        for (auto &root : _grassRoots) {
            root->update(dt);
        }
        for (auto &root : _soundRoots) {
            root->update(dt);
        }
    }
    if (!_activeCamera) {
        return;
    }
    cullRoots();
    refresh();
    updateLighting();
    updateShadowLight(dt);
    updateFlareLights();
    updateSounds();
    prepareOpaqueLeafs();
    prepareTransparentLeafs();
}

void SceneGraph::cullRoots() {
    for (auto &root : _modelRoots) {
        bool culled =
            !root->isEnabled() ||
            root->getSquareDistanceTo(*_activeCamera) > root->drawDistance() * root->drawDistance() ||
            (root->isCullable() && !_activeCamera->isInFrustum(*root));

        root->setCulled(culled);
    }
}

void SceneGraph::updateLighting() {
    // Find closest lights and create a lookup
    auto closestLights = computeClosestLights(kMaxLights, [](auto &light, float distance2) {
        float radius2 = light.radius() * light.radius();
        return distance2 < radius2 * radius2 + kLightRadiusBias2;
    });
    std::set<LightSceneNode *> lookup;
    for (auto &light : closestLights) {
        lookup.insert(light);
    }
    // De-activate active lights, unless found in a lookup. Active lights are removed from the lookup
    for (auto &light : _activeLights) {
        if (lookup.count(light) == 0) {
            light->setActive(false);
        } else {
            lookup.erase(light);
        }
    }
    // Remove active lights that are inactive and completely faded
    for (auto it = _activeLights.begin(); it != _activeLights.end();) {
        auto light = *it;
        if ((!light->isActive() && light->strength() == 0.0f) || (!light->model().isEnabled())) {
            it = _activeLights.erase(it);
        } else {
            ++it;
        }
    }
    // Add closest lights to active lights
    for (auto &light : lookup) {
        if (_activeLights.size() >= kMaxLights) {
            return;
        }
        light->setActive(true);
        _activeLights.push_back(light);
    }
}

void SceneGraph::updateShadowLight(float dt) {
    auto closestLights = computeClosestLights(1, [this](auto &light, float distance2) {
        if (!light.modelNode().light()->shadow) {
            return false;
        }
        float radius = light.radius();
        return distance2 < radius * radius;
    });
    if (_shadowLight) {
        if (closestLights.empty() || _shadowLight != closestLights.front()) {
            _shadowActive = false;
        }
        if (_shadowActive) {
            _shadowStrength = glm::min(1.0f, _shadowStrength + kShadowFadeSpeed * dt);
        } else {
            _shadowStrength = glm::max(0.0f, _shadowStrength - kShadowFadeSpeed * dt);
            if (_shadowStrength == 0.0f) {
                _shadowLight = nullptr;
            }
        }
    }
    if (!_shadowLight && !closestLights.empty()) {
        _shadowLight = closestLights.front();
        _shadowActive = true;
    }
}

void SceneGraph::updateFlareLights() {
    _flareLights = computeClosestLights(kMaxFlareLights, [](auto &light, float distance2) {
        if (light.modelNode().light()->flares.empty()) {
            return false;
        }
        float radius = light.modelNode().light()->flareRadius;
        return distance2 < radius * radius;
    });
}

void SceneGraph::updateSounds() {
    std::vector<std::pair<SoundSceneNode *, float>> distances;
    glm::vec3 cameraPos(_activeCamera->localTransform()[3]);

    // For each sound, calculate its distance to the camera
    for (auto &root : _soundRoots) {
        root->setAudible(false);
        if (!root->isEnabled()) {
            continue;
        }
        float dist2 = root->getSquareDistanceTo(cameraPos);
        float maxDist2 = root->maxDistance() * root->maxDistance();
        if (dist2 > maxDist2) {
            continue;
        }
        distances.push_back(std::make_pair(root.get(), dist2));
    }

    // Take up to N most closest sounds to the camera
    sort(distances.begin(), distances.end(), [](auto &left, auto &right) {
        int leftPriority = left.first->priority();
        int rightPriority = right.first->priority();
        if (leftPriority < rightPriority) {
            return true;
        }
        if (leftPriority > rightPriority) {
            return false;
        }
        return left.second < right.second;
    });
    if (distances.size() > kMaxSoundCount) {
        distances.erase(distances.begin() + kMaxSoundCount, distances.end());
    }

    // Mark closest sounds as audible
    for (auto &pair : distances) {
        pair.first->setAudible(true);
    }
}

void SceneGraph::refresh() {
    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _shadowMeshes.clear();
    _lights.clear();
    _emitters.clear();

    for (auto &root : _modelRoots) {
        refreshFromNode(*root);
    }
}

void SceneGraph::refreshFromNode(SceneNode &node) {
    bool propagate = true;

    switch (node.type()) {
    case SceneNodeType::Model: {
        // Ignore models that have been culled
        auto &model = static_cast<ModelSceneNode &>(node);
        if (model.isCulled()) {
            propagate = false;
        }
        break;
    }
    case SceneNodeType::Mesh: {
        // For model nodes, determine whether they should be rendered and cast shadows
        auto &modelNode = static_cast<MeshSceneNode &>(node);
        if (modelNode.shouldRender()) {
            // Sort model nodes into transparent and opaque
            if (modelNode.isTransparent()) {
                _transparentMeshes.push_back(&modelNode);
            } else {
                _opaqueMeshes.push_back(&modelNode);
            }
        }
        if (modelNode.shouldCastShadows()) {
            _shadowMeshes.push_back(&modelNode);
        }
        break;
    }
    case SceneNodeType::Light:
        _lights.push_back(static_cast<LightSceneNode *>(&node));
        break;
    case SceneNodeType::Emitter:
        _emitters.push_back(static_cast<EmitterSceneNode *>(&node));
        break;
    default:
        break;
    }

    if (propagate) {
        for (auto &child : node.children()) {
            refreshFromNode(*child);
        }
    }
}

void SceneGraph::prepareOpaqueLeafs() {
    _opaqueLeafs.clear();

    std::vector<SceneNode *> bucket;
    auto camera = _activeCamera->camera();

    // Group grass clusters into buckets without sorting
    for (auto &grass : _grassRoots) {
        if (!grass->isEnabled()) {
            continue;
        }
        for (auto &child : grass->children()) {
            if (child->type() != SceneNodeType::GrassCluster) {
                continue;
            }
            auto cluster = static_cast<GrassClusterSceneNode *>(child);
            if (!camera->isInFrustum(cluster->getOrigin())) {
                continue;
            }
            if (bucket.size() >= kMaxGrassClusters) {
                _opaqueLeafs.push_back(std::make_pair(grass.get(), bucket));
                bucket.clear();
            }
            bucket.push_back(cluster);
        }
        if (!bucket.empty()) {
            _opaqueLeafs.push_back(std::make_pair(grass.get(), bucket));
            bucket.clear();
        }
    }
}

void SceneGraph::prepareTransparentLeafs() {
    _transparentLeafs.clear();

    auto camera = _activeCamera->camera();

    // Add meshes and emitters to transparent leafs
    std::vector<SceneNode *> leafs;
    for (auto &mesh : _transparentMeshes) {
        leafs.push_back(mesh);
    }
    for (auto &emitter : _emitters) {
        for (auto &child : emitter->children()) {
            if (child->type() != SceneNodeType::Particle) {
                continue;
            }
            auto particle = static_cast<ParticleSceneNode *>(child);
            if (!camera->isInFrustum(particle->getOrigin())) {
                continue;
            }
            leafs.push_back(particle);
        }
    }

    // Group transparent leafs into buckets
    SceneNode *bucketParent = nullptr;
    std::vector<SceneNode *> bucket;
    for (auto leaf : leafs) {
        SceneNode *parent = leaf->parent();
        if (leaf->type() == SceneNodeType::Mesh) {
            parent = &static_cast<MeshSceneNode *>(leaf)->model();
        }
        if (!bucket.empty()) {
            int maxCount = 1;
            if (parent->type() == SceneNodeType::Emitter) {
                maxCount = kMaxParticles;
            } else if (parent->type() == SceneNodeType::Grass) {
                maxCount = kMaxGrassClusters;
            }
            if (bucketParent != parent || bucket.size() >= maxCount) {
                _transparentLeafs.push_back(std::make_pair(bucketParent, bucket));
                bucket.clear();
            }
        }
        bucketParent = parent;
        bucket.push_back(leaf);
    }
    if (bucketParent && !bucket.empty()) {
        _transparentLeafs.push_back(std::make_pair(bucketParent, bucket));
    }
}

void SceneGraph::drawShadows() {
    if (!_activeCamera) {
        return;
    }
    _graphicsSvc.context.withFaceCulling(CullFaceMode::Front, [this]() {
        for (auto &mesh : _shadowMeshes) {
            mesh->drawShadow();
        }
    });
}

void SceneGraph::drawOpaque() {
    if (!_activeCamera) {
        return;
    }
    if (_drawWalkmeshes || _drawTriggers) {
        _graphicsSvc.uniforms.setWalkmesh([this](auto &walkmesh) {
            for (int i = 0; i < kMaxWalkmeshMaterials - 1; ++i) {
                walkmesh.materials[i] = _walkableSurfaces.count(i) > 0 ? glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
            walkmesh.materials[kMaxWalkmeshMaterials - 1] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // triggers
        });
    }

    // Draw opaque meshes
    for (auto &mesh : _opaqueMeshes) {
        mesh->draw();
    }
    // Draw opaque leafs
    for (auto &[node, leafs] : _opaqueLeafs) {
        node->drawLeafs(leafs);
    }

    if (_drawAABB) {
        for (auto &model : _modelRoots) {
            model->drawAABB();
        }
    }
    if (_drawWalkmeshes) {
        for (auto &walkmesh : _walkmeshRoots) {
            if (walkmesh->isEnabled()) {
                walkmesh->draw();
            }
        }
    }
    if (_drawTriggers) {
        for (auto &trigger : _triggerRoots) {
            trigger->draw();
        }
    }
}

void SceneGraph::drawTransparent() {
    if (!_activeCamera || _drawWalkmeshes) {
        return;
    }
    // Draw transparent leafs (incl. meshes)
    for (auto &[node, leafs] : _transparentLeafs) {
        node->drawLeafs(leafs);
    }
}

void SceneGraph::drawLensFlares() {
    // Draw lens flares
    if (_flareLights.empty() || _drawWalkmeshes) {
        return;
    }
    _graphicsSvc.context.withDepthTest(DepthTestMode::None, [this]() {
        for (auto &light : _flareLights) {
            Collision collision;
            if (testLineOfSight(_activeCamera->getOrigin(), light->getOrigin(), collision)) {
                continue;
            }
            light->drawLensFlare(light->modelNode().light()->flares.front());
        }
    });
}

std::vector<LightSceneNode *> SceneGraph::computeClosestLights(int count, const std::function<bool(const LightSceneNode &, float)> &pred) const {
    // Compute distance from each light to the camera
    std::vector<std::pair<LightSceneNode *, float>> distances;
    for (auto &light : _lights) {
        float distance2 = light->getSquareDistanceTo(*_activeCamera);
        if (!pred(*light, distance2)) {
            continue;
        }
        distances.push_back(std::make_pair(light, distance2));
    }

    // Sort lights by distance to the camera. Directional lights are prioritizied
    sort(distances.begin(), distances.end(), [](auto &a, auto &b) {
        auto aLight = a.first;
        auto bLight = b.first;
        if (aLight->isDirectional() && !bLight->isDirectional()) {
            return true;
        }
        if (!aLight->isDirectional() && bLight->isDirectional()) {
            return false;
        }
        float aDistance = a.second;
        float bDistance = b.second;
        return aDistance < bDistance;
    });

    // Keep up to maximum number of lights
    if (distances.size() > count) {
        distances.erase(distances.begin() + count, distances.end());
    }

    std::vector<LightSceneNode *> lights;
    for (auto &light : distances) {
        lights.push_back(light.first);
    }
    return lights;
}

bool SceneGraph::testElevation(const glm::vec2 &position, Collision &outCollision) const {
    static glm::vec3 down(0.0f, 0.0f, -1.0f);

    glm::vec3 origin(position, kElevationTestZ);

    for (auto &root : _walkmeshRoots) {
        if (!root->isEnabled()) {
            continue;
        }
        if (!root->walkmesh().isAreaWalkmesh()) {
            float distance2 = root->getSquareDistanceTo2D(position);
            if (distance2 > kMaxCollisionDistanceWalk2) {
                continue;
            }
        }
        auto objSpaceOrigin = glm::vec3(root->absoluteTransformInverse() * glm::vec4(origin, 1.0f));
        float distance = 0.0f;
        auto face = root->walkmesh().raycast(_walkcheckSurfaces, objSpaceOrigin, down, 2.0f * kElevationTestZ, distance);
        if (face) {
            if (_walkableSurfaces.count(face->material) == 0) {
                // non-walkable
                return false;
            }
            outCollision.user = root->user();
            outCollision.intersection = origin + distance * down;
            outCollision.normal = root->absoluteTransform() * glm::vec4(face->normal, 0.0f);
            outCollision.material = face->material;
            return true;
        }
    }

    return false;
}

bool SceneGraph::testLineOfSight(const glm::vec3 &origin, const glm::vec3 &dest, Collision &outCollision) const {
    glm::vec3 originToDest(dest - origin);
    glm::vec3 dir(glm::normalize(originToDest));
    float maxDistance = glm::length(originToDest);
    float minDistance = std::numeric_limits<float>::max();

    for (auto &root : _walkmeshRoots) {
        if (!root->isEnabled()) {
            continue;
        }
        if (!root->walkmesh().isAreaWalkmesh()) {
            float distance2 = root->getSquareDistanceTo(origin);
            if (distance2 > kMaxCollisionDistanceLineOfSight2) {
                continue;
            }
        }
        glm::vec3 objSpaceOrigin(root->absoluteTransformInverse() * glm::vec4(origin, 1.0f));
        glm::vec3 objSpaceDir(root->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance = 0.0f;
        auto face = root->walkmesh().raycast(_lineOfSightSurfaces, objSpaceOrigin, objSpaceDir, maxDistance, distance);
        if (!face || distance > minDistance) {
            continue;
        }
        outCollision.user = root->user();
        outCollision.intersection = origin + distance * dir;
        outCollision.normal = root->absoluteTransform() * glm::vec4(face->normal, 0.0f);
        outCollision.material = face->material;
        minDistance = distance;
    }

    return minDistance != std::numeric_limits<float>::max();
}

bool SceneGraph::testWalk(const glm::vec3 &origin, const glm::vec3 &dest, const IUser *excludeUser, Collision &outCollision) const {
    glm::vec3 originToDest(dest - origin);
    glm::vec3 dir(glm::normalize(originToDest));
    float maxDistance = glm::length(originToDest);
    float minDistance = std::numeric_limits<float>::max();

    for (auto &root : _walkmeshRoots) {
        if (!root->isEnabled() || root->user() == excludeUser) {
            continue;
        }
        if (!root->walkmesh().isAreaWalkmesh()) {
            float distance2 = root->getSquareDistanceTo(origin);
            if (distance2 > kMaxCollisionDistanceWalk2) {
                continue;
            }
        }
        glm::vec3 objSpaceOrigin(root->absoluteTransformInverse() * glm::vec4(origin, 1.0f));
        glm::vec3 objSpaceDir(root->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance = 0.0f;
        auto face = root->walkmesh().raycast(_walkcheckSurfaces, objSpaceOrigin, objSpaceDir, kMaxCollisionDistanceWalk, distance);
        if (!face || distance > maxDistance || distance > minDistance) {
            continue;
        }
        outCollision.user = root->user();
        outCollision.intersection = origin + distance * dir;
        outCollision.normal = root->absoluteTransform() * glm::vec4(face->normal, 0.0f);
        outCollision.material = face->material;
        minDistance = distance;
    }

    return minDistance != std::numeric_limits<float>::max();
}

ModelSceneNode *SceneGraph::pickModelAt(int x, int y, IUser *except) const {
    if (!_activeCamera) {
        return nullptr;
    }

    auto camera = _activeCamera->camera();
    glm::vec4 viewport(0.0f, 0.0f, _graphicsOpt.width, _graphicsOpt.height);
    glm::vec3 start(glm::unProject(glm::vec3(x, _graphicsOpt.height - y, 0.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 end(glm::unProject(glm::vec3(x, _graphicsOpt.height - y, 1.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 dir(glm::normalize(end - start));

    std::vector<std::pair<ModelSceneNode *, float>> distances;
    for (auto &model : _modelRoots) {
        if (!model->isPickable() || model->user() == except) {
            continue;
        }
        if (model->getSquareDistanceTo(start) > kMaxCollisionDistanceLineOfSight2) {
            continue;
        }
        auto objSpaceStart = model->absoluteTransformInverse() * glm::vec4(start, 1.0f);
        auto objSpaceInvDir = 1.0f / (model->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance;
        if (model->aabb().raycast(objSpaceStart, objSpaceInvDir, kMaxCollisionDistanceLineOfSight, distance) && distance > 0.0f) {
            Collision collision;
            if (testLineOfSight(start, start + distance * dir, collision) && collision.user != model->user()) {
                continue;
            }
            distances.push_back(std::make_pair(model.get(), distance));
        }
    }
    if (distances.empty()) {
        return nullptr;
    }
    sort(distances.begin(), distances.end(), [](auto &left, auto &right) { return left.second < right.second; });

    return distances[0].first;
}

void SceneGraph::fillLightsUniforms() {
    _graphicsSvc.uniforms.setLights([this](auto &lighting) {
        lighting.numLights = static_cast<int>(_activeLights.size());
        for (size_t i = 0; i < _activeLights.size(); ++i) {
            LightUniforms &shaderLight = lighting.lights[i];
            shaderLight.position = glm::vec4(_activeLights[i]->getOrigin(), _activeLights[i]->isDirectional() ? 0.0f : 1.0f);
            shaderLight.color = glm::vec4(_activeLights[i]->color(), 1.0f);
            shaderLight.multiplier = _activeLights[i]->multiplier() * _activeLights[i]->strength();
            shaderLight.radius = _activeLights[i]->radius();
            shaderLight.ambientOnly = static_cast<int>(_activeLights[i]->modelNode().light()->ambientOnly);
            shaderLight.dynamicType = _activeLights[i]->modelNode().light()->dynamicType;
        }
    });
}

std::shared_ptr<CameraSceneNode> SceneGraph::newCamera() {
    return newSceneNode<CameraSceneNode>();
}

std::shared_ptr<DummySceneNode> SceneGraph::newDummy(ModelNode &modelNode) {
    return newSceneNode<DummySceneNode, ModelNode &>(modelNode);
}

std::shared_ptr<ModelSceneNode> SceneGraph::newModel(Model &model, ModelUsage usage) {
    return newSceneNode<ModelSceneNode, Model &, ModelUsage>(model, usage);
}

std::shared_ptr<WalkmeshSceneNode> SceneGraph::newWalkmesh(Walkmesh &walkmesh) {
    return newSceneNode<WalkmeshSceneNode, Walkmesh &>(walkmesh);
}

std::shared_ptr<SoundSceneNode> SceneGraph::newSound() {
    return newSceneNode<SoundSceneNode>();
}

std::shared_ptr<MeshSceneNode> SceneGraph::newMesh(ModelSceneNode &model, ModelNode &modelNode) {
    return newSceneNode<MeshSceneNode, ModelSceneNode &, ModelNode &>(model, modelNode);
}

std::shared_ptr<LightSceneNode> SceneGraph::newLight(ModelSceneNode &model, ModelNode &modelNode) {
    return newSceneNode<LightSceneNode, ModelSceneNode &, ModelNode &>(model, modelNode);
}

std::shared_ptr<TriggerSceneNode> SceneGraph::newTrigger(std::vector<glm::vec3> geometry) {
    return newSceneNode<TriggerSceneNode, std::vector<glm::vec3>>(std::move(geometry));
}

std::shared_ptr<EmitterSceneNode> SceneGraph::newEmitter(ModelNode &modelNode) {
    return newSceneNode<EmitterSceneNode, ModelNode &>(modelNode);
}

std::shared_ptr<ParticleSceneNode> SceneGraph::newParticle(EmitterSceneNode &emitter) {
    return newSceneNode<ParticleSceneNode, EmitterSceneNode &>(emitter);
}

std::shared_ptr<GrassSceneNode> SceneGraph::newGrass(GrassProperties properties, ModelNode &aabbNode) {
    return newSceneNode<GrassSceneNode, GrassProperties, ModelNode &>(properties, aabbNode);
}

std::shared_ptr<GrassClusterSceneNode> SceneGraph::newGrassCluster(GrassSceneNode &grass) {
    return newSceneNode<GrassClusterSceneNode>();
}

} // namespace scene

} // namespace reone
