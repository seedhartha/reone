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

#include "graph.h"

#include "../graphics/context.h"
#include "../graphics/mesh.h"
#include "../graphics/meshes.h"
#include "../graphics/walkmesh.h"

#include "collision.h"
#include "node/camera.h"
#include "node/emitter.h"
#include "node/grass.h"
#include "node/grasscluster.h"
#include "node/light.h"
#include "node/mesh.h"
#include "node/model.h"
#include "node/particle.h"
#include "node/sound.h"
#include "node/walkmesh.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kElevationTestZ = 1024.0f;
static constexpr int kMaxSoundCount = 4;

static constexpr float kMaxCollisionDistance = 8.0f;
static constexpr float kMaxCollisionDistance2 = kMaxCollisionDistance * kMaxCollisionDistance;

void SceneGraph::clear() {
    _modelRoots.clear();
    _walkmeshRoots.clear();
    _soundRoots.clear();
    _grassRoots.clear();
    _closestLights.clear();
}

void SceneGraph::addRoot(shared_ptr<ModelSceneNode> node) {
    _modelRoots.insert(move(node));
}

void SceneGraph::addRoot(shared_ptr<WalkmeshSceneNode> node) {
    _walkmeshRoots.insert(move(node));
}

void SceneGraph::addRoot(shared_ptr<SoundSceneNode> node) {
    _soundRoots.insert(move(node));
}

void SceneGraph::addRoot(shared_ptr<GrassSceneNode> node) {
    _grassRoots.insert(move(node));
}

void SceneGraph::removeRoot(const shared_ptr<ModelSceneNode> &node) {
    _modelRoots.erase(node);
}

void SceneGraph::removeRoot(const shared_ptr<WalkmeshSceneNode> &node) {
    _walkmeshRoots.erase(node);
}

void SceneGraph::removeRoot(const shared_ptr<SoundSceneNode> &node) {
    _soundRoots.erase(node);
}

void SceneGraph::removeRoot(const shared_ptr<GrassSceneNode> &node) {
    _grassRoots.erase(node);
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
    refreshNodeLists();
    updateLighting();
    prepareTransparentMeshes();
    prepareLeafs();
    updateSounds();
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
    if (!_lightingRefNode) {
        return;
    }

    // Find next closest lights and create a lookup
    vector<LightSceneNode *> lights(getLightsAt(kMaxLights));
    set<LightSceneNode *> lookup;
    for (auto &light : lights) {
        lookup.insert(light);
    }
    // Mark current closest lights as inactive, unless found in a lookup. Lookup will contain new lights.
    for (auto &light : _closestLights) {
        if (lookup.count(light) == 0) {
            light->setActive(false);
        } else {
            lookup.erase(light);
        }
    }
    // Erase current closest lights that are inactive and completely faded
    for (auto it = _closestLights.begin(); it != _closestLights.end();) {
        LightSceneNode *light = *it;
        if (!light->isActive() && light->fadeFactor() == 1.0f) {
            it = _closestLights.erase(it);
        } else {
            ++it;
        }
    }
    // Add next closest lights to current
    for (size_t i = 0; i < lights.size() && _closestLights.size() < kMaxLights; ++i) {
        if (lookup.count(lights[i]) > 0) {
            lights[i]->setActive(true);
            lights[i]->setFadeFactor(1.0f);
            _closestLights.push_back(lights[i]);
        }
    }
    // Update shadow light
    _shadowLight = nullptr;
    for (auto &light : _closestLights) {
        if (light->modelNode().light()->shadow) {
            _shadowLight = light;
            break;
        }
    }
}

void SceneGraph::refreshNodeLists() {
    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _shadowMeshes.clear();
    _lights.clear();
    _emitters.clear();

    for (auto &root : _modelRoots) {
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
    default:
        break;
    }

    if (propagate) {
        for (auto &child : node->children()) {
            refreshFromSceneNode(child);
        }
    }
}

void SceneGraph::prepareTransparentMeshes() {
    // Sort transparent meshes by transparency and distance to camera, so as to ensure correct blending
    glm::vec3 cameraPosition(_activeCamera->absoluteTransform()[3]);
    unordered_map<SceneNode *, float> meshToCamera;
    for (auto &mesh : _transparentMeshes) {
        meshToCamera.insert(make_pair(mesh, mesh->getSquareDistanceTo(cameraPosition)));
    }
    sort(_transparentMeshes.begin(), _transparentMeshes.end(), [&meshToCamera](auto &left, auto &right) {
        int leftTransparency = left->modelNode().mesh()->transparency;
        int rightTransparency = right->modelNode().mesh()->transparency;

        if (leftTransparency < rightTransparency)
            return true;
        if (leftTransparency > rightTransparency)
            return false;

        float leftDistance = meshToCamera.find(left)->second;
        float rightDistance = meshToCamera.find(right)->second;

        return leftDistance > rightDistance;
    });
}

void SceneGraph::prepareLeafs() {
    static glm::vec4 viewport(-1.0f, -1.0f, 1.0f, 1.0f);

    vector<pair<SceneNode *, float>> leafs;
    auto cameraPos = _activeCamera->getOrigin();

    // Add grass clusters
    for (auto &grass : _grassRoots) {
        if (!grass->isEnabled()) {
            continue;
        }
        for (auto &child : grass->children()) {
            if (child->type() != SceneNodeType::GrassCluster) {
                continue;
            }
            auto cluster = static_cast<GrassClusterSceneNode *>(child.get());
            glm::vec3 screen(glm::project(cluster->getOrigin(), _activeCamera->view(), _activeCamera->projection(), viewport));
            if (screen.z >= 0.5f && glm::abs(screen.x) <= 1.0f && glm::abs(screen.y) <= 1.0f) {
                leafs.push_back(make_pair(cluster, screen.z));
            }
        }
    }

    // Add particles
    for (auto &emitter : _emitters) {
        for (auto &child : emitter->children()) {
            if (child->type() != SceneNodeType::Particle) {
                continue;
            }
            auto particle = static_cast<ParticleSceneNode *>(child.get());
            glm::vec3 screen(glm::project(particle->getOrigin(), _activeCamera->view(), _activeCamera->projection(), viewport));
            if (screen.z >= 0.5f && glm::abs(screen.x) <= 1.0f && glm::abs(screen.y) <= 1.0f) {
                leafs.push_back(make_pair(particle, screen.z));
            }
        }
    }

    // Sort leafs back to front
    sort(leafs.begin(), leafs.end(), [](auto &left, auto &right) { return left.second > right.second; });

    // Group leafs into buckets
    _leafs.clear();
    vector<SceneNode *> nodeLeafs;
    for (auto &leafDepth : leafs) {
        if (!nodeLeafs.empty()) {
            _leafs.push_back(make_pair(nodeLeafs[0]->parent(), nodeLeafs));
            nodeLeafs.clear();
        }
        nodeLeafs.push_back(leafDepth.first);
    }
    if (!nodeLeafs.empty()) {
        _leafs.push_back(make_pair(nodeLeafs[0]->parent(), nodeLeafs));
    }
}

void SceneGraph::updateSounds() {
    vector<pair<SoundSceneNode *, float>> distances;
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
        distances.push_back(make_pair(root.get(), dist2));
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

void SceneGraph::draw(bool shadowPass) {
    static glm::vec3 white {1.0f, 1.0f, 1.0f};
    static glm::vec3 red {1.0f, 0.0f, 0.0f};

    if (!_activeCamera) {
        return;
    }

    // Render only shadow meshes on shadow pass
    if (shadowPass) {
        for (auto &mesh : _shadowMeshes) {
            mesh->drawSingle(true);
        }
        return;
    }

    _graphicsContext.setBackFaceCullingEnabled(true);

    // Render opaque meshes
    for (auto &mesh : _opaqueMeshes) {
        mesh->drawSingle(false);
    }

    // Render transparent meshes
    for (auto &mesh : _transparentMeshes) {
        mesh->drawSingle(false);
    }

    _graphicsContext.setBackFaceCullingEnabled(false);

    // Render particles and grass clusters
    for (auto &nodeLeaf : _leafs) {
        int count = nodeLeaf.first->type() == SceneNodeType::Grass && nodeLeaf.second.size() > kMaxGrassClusters ? kMaxGrassClusters : -1;
        nodeLeaf.first->drawElements(nodeLeaf.second, count);
    }

    // Render lens flares
    for (auto &light : _lights) {
        // Ignore lights that are too far away or outside of camera frustum
        float flareRadius = light->modelNode().light()->flareRadius;
        if (_activeCamera->getSquareDistanceTo(*light) > flareRadius * flareRadius ||
            !_activeCamera->isInFrustum(light->absoluteTransform()[3]))
            continue;

        for (auto &flare : light->modelNode().light()->flares) {
            light->drawLensFlares(flare);
        }
    }
}

vector<LightSceneNode *> SceneGraph::getLightsAt(
    int count,
    function<bool(const LightSceneNode &)> predicate) const {

    if (!_lightingRefNode) {
        return vector<LightSceneNode *>();
    }

    // Compute distances between each light and the reference node
    vector<pair<LightSceneNode *, float>> distances;

    for (auto &light : _lights) {
        if (!predicate(*light))
            continue;

        // Only account for lights whose distance to the reference node is
        // within range of the light.
        float distance = light->getSquareDistanceTo(*_lightingRefNode);
        if (distance > light->radius() * light->radius())
            continue;

        distances.push_back(make_pair(light, distance));
    }

    // Sort lights by priority and radius
    sort(distances.begin(), distances.end(), [](auto &left, auto &right) {
        LightSceneNode *leftLight = left.first;
        LightSceneNode *rightLight = right.first;
        int leftPriority = leftLight->modelNode().light()->priority;
        int rightPriority = rightLight->modelNode().light()->priority;
        if (leftPriority < rightPriority) {
            return true;
        }
        if (leftPriority > rightPriority) {
            return false;
        }
        float leftDistance = left.second;
        float rightDistance = right.second;
        return leftDistance < rightDistance;
    });

    // Keep first count lights only
    if (distances.size() > count) {
        distances.erase(distances.begin() + count, distances.end());
    }

    vector<LightSceneNode *> result;
    for (auto &light : distances) {
        result.push_back(light.first);
    }

    return move(result);
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
            if (distance2 > kMaxCollisionDistance2) {
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

bool SceneGraph::testObstacle(const glm::vec3 &origin, const glm::vec3 &dest, const IUser *excludeUser, Collision &outCollision) const {
    glm::vec3 originToDest(dest - origin);
    glm::vec3 dir(glm::normalize(originToDest));
    float maxDistance = glm::length(originToDest);
    float minDistance = numeric_limits<float>::max();

    for (auto &root : _walkmeshRoots) {
        if (root->user() == excludeUser) {
            continue;
        }
        if (!root->isEnabled()) {
            continue;
        }
        if (!root->walkmesh().isAreaWalkmesh()) {
            float distance2 = root->getSquareDistanceTo(origin);
            if (distance2 > kMaxCollisionDistance2) {
                continue;
            }
        }
        glm::vec3 objSpaceOrigin(root->absoluteTransformInverse() * glm::vec4(origin, 1.0f));
        glm::vec3 objSpaceDir(root->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance = 0.0f;
        auto face = root->walkmesh().raycast(_walkcheckSurfaces, objSpaceOrigin, objSpaceDir, 2.0f * kElevationTestZ, distance);
        if (!face || distance > maxDistance || distance > minDistance) {
            continue;
        }
        outCollision.user = root->user();
        outCollision.intersection = origin + distance * dir;
        outCollision.normal = root->absoluteTransform() * glm::vec4(face->normal, 0.0f);
        outCollision.material = face->material;
        minDistance = distance;
    }

    return minDistance != numeric_limits<float>::max();
}

shared_ptr<ModelSceneNode> SceneGraph::pickModelAt(int x, int y, IUser *except) const {
    if (!_activeCamera) {
        return nullptr;
    }
    glm::vec4 viewport(0.0f, 0.0f, _options.width, _options.height);
    glm::vec3 start(glm::unProject(glm::vec3(x, _options.height - y, 0.0f), _activeCamera->view(), _activeCamera->projection(), viewport));
    glm::vec3 end(glm::unProject(glm::vec3(x, _options.height - y, 1.0f), _activeCamera->view(), _activeCamera->projection(), viewport));
    glm::vec3 dir(glm::normalize(end - start));

    vector<pair<shared_ptr<ModelSceneNode>, float>> distances;
    for (auto &model : _modelRoots) {
        if (!model->isPickable() || model->user() == except) {
            continue;
        }
        // Distance to object must not exceed maximum collision distance
        if (model->getSquareDistanceTo(start) > kMaxCollisionDistance2) {
            continue;
        }
        // Test object AABB (object space)
        glm::vec3 objSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec3 objSpaceDir(model->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance;
        if (model->aabb().raycast(objSpaceStart, objSpaceDir, kMaxCollisionDistance, distance)) {
            distances.push_back(make_pair(model, distance));
        }
    }
    if (distances.empty()) {
        return nullptr;
    }
    std::sort(distances.begin(), distances.end(), [](auto &left, auto &right) { return left.second < right.second; });

    return distances[0].first;
}

unique_ptr<DummySceneNode> SceneGraph::newDummy(shared_ptr<ModelNode> modelNode) {
    return make_unique<DummySceneNode>(move(modelNode), *this, _graphicsContext, _meshes, _shaders);
}

unique_ptr<CameraSceneNode> SceneGraph::newCamera(glm::mat4 projection) {
    return make_unique<CameraSceneNode>(move(projection), *this);
}

unique_ptr<SoundSceneNode> SceneGraph::newSound() {
    return make_unique<SoundSceneNode>(*this, _audioPlayer);
}

unique_ptr<ModelSceneNode> SceneGraph::newModel(shared_ptr<Model> model, ModelUsage usage, IAnimationEventListener *animEventListener) {
    return make_unique<ModelSceneNode>(
        move(model),
        usage,
        *this,
        _graphicsContext,
        _meshes,
        _shaders,
        _textures,
        animEventListener);
}

unique_ptr<WalkmeshSceneNode> SceneGraph::newWalkmesh(shared_ptr<Walkmesh> walkmesh) {
    return make_unique<WalkmeshSceneNode>(move(walkmesh), *this);
}

unique_ptr<GrassSceneNode> SceneGraph::newGrass(float density, float quadSize, glm::vec4 probabilities, set<uint32_t> materials, shared_ptr<Texture> texture, shared_ptr<ModelNode> aabbNode) {
    return make_unique<GrassSceneNode>(
        density,
        quadSize,
        move(probabilities),
        move(materials),
        move(texture),
        move(aabbNode),
        *this,
        _graphicsContext,
        _meshes,
        _shaders);
}

} // namespace scene

} // namespace reone
