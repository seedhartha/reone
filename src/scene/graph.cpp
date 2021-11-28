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
static constexpr float kMaxGrassDistance = 16.0f;
static constexpr int kMaxSoundCount = 4;

static const bool g_debugAABB = false;

void SceneGraph::clear() {
    _roots.clear();
    _closestLights.clear();
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
    for (auto &root : _roots) {
        if (root->type() != SceneNodeType::Model) {
            continue;
        }
        auto modelRoot = static_pointer_cast<ModelSceneNode>(root);

        bool culled =
            !modelRoot->isVisible() ||
            modelRoot->getDistanceTo2(*_activeCamera) > modelRoot->drawDistance() * modelRoot->drawDistance() ||
            (modelRoot->isCullable() && !_activeCamera->isInFrustum(*root));

        modelRoot->setCulled(culled);
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

void SceneGraph::prepareTransparentMeshes() {
    // Sort transparent meshes by transparency and distance to camera, so as to ensure correct blending
    glm::vec3 cameraPosition(_activeCamera->absoluteTransform()[3]);
    unordered_map<SceneNode *, float> meshToCamera;
    for (auto &mesh : _transparentMeshes) {
        meshToCamera.insert(make_pair(mesh, mesh->getDistanceTo2(cameraPosition)));
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

    vector<pair<SceneNode *, float>> elements;
    glm::vec3 cameraPos(_activeCamera->absoluteTransform()[3]);

    // Add grass clusters
    for (auto &grass : _grass) {
        float grassDistance2 = kMaxGrassDistance * kMaxGrassDistance;
        size_t numLeafs = elements.size();
        for (auto &grassChild : grass->children()) {
            auto cluster = static_pointer_cast<GrassClusterSceneNode>(grassChild);
            float distance2 = glm::distance2(cameraPos, cluster->position());
            if (distance2 <= grassDistance2) {
                glm::vec3 screen(glm::project(cluster->position(), _activeCamera->view(), _activeCamera->projection(), viewport));
                if (screen.z >= 0.5f && glm::abs(screen.x) <= 1.0f && glm::abs(screen.y) <= 1.0f) {
                    elements.push_back(make_pair(cluster.get(), screen.z));
                }
            }
        }
    }

    // Add particles
    for (auto &emitter : _emitters) {
        glm::mat4 modelView(_activeCamera->view() * emitter->absoluteTransform());
        for (auto &emitterChild : emitter->children()) {
            auto particle = static_pointer_cast<ParticleSceneNode>(emitterChild);
            glm::vec3 screen(glm::project(particle->position(), modelView, _activeCamera->projection(), viewport));
            if (screen.z >= 0.5f && glm::abs(screen.x) <= 1.0f && glm::abs(screen.y) <= 1.0f) {
                elements.push_back(make_pair(particle.get(), screen.z));
            }
        }
    }

    // Sort elements back to front
    sort(elements.begin(), elements.end(), [](auto &left, auto &right) { return left.second > right.second; });

    // Group elements into buckets
    _elements.clear();
    vector<SceneNode *> nodeElements;
    for (auto &elementDepth : elements) {
        if (!nodeElements.empty()) {
            _elements.push_back(make_pair(nodeElements[0]->parent(), nodeElements));
            nodeElements.clear();
        }
        nodeElements.push_back(elementDepth.first);
    }
    if (!nodeElements.empty()) {
        _elements.push_back(make_pair(nodeElements[0]->parent(), nodeElements));
    }
}

void SceneGraph::updateSounds() {
    vector<pair<SoundSceneNode *, float>> distances;
    glm::vec3 cameraPos(_activeCamera->localTransform()[3]);

    // For each sound, calculate its distance to the camera
    for (auto &root : _roots) {
        if (root->type() != SceneNodeType::Sound) {
            continue;
        }
        auto sound = static_pointer_cast<SoundSceneNode>(root);
        sound->setAudible(false);
        if (!sound->isEnabled()) {
            continue;
        }
        float dist2 = sound->getDistanceTo2(cameraPos);
        float maxDist2 = sound->maxDistance() * sound->maxDistance();
        if (dist2 > maxDist2) {
            continue;
        }
        distances.push_back(make_pair(sound.get(), dist2));
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

    if (!_activeCamera)
        return;

    if (shadowPass) {
        // Render shadow meshes
        for (auto &mesh : _shadowMeshes) {
            mesh->drawSingle(true);
        }
        return;
    }

    _context.setBackFaceCullingEnabled(true);

    // Render opaque meshes
    for (auto &mesh : _opaqueMeshes) {
        mesh->drawSingle(false);
    }

    if (g_debugAABB) {
        for (auto &root : _roots) {
            bool walkmesh = root->type() == SceneNodeType::Walkmesh;
            if (walkmesh && !static_pointer_cast<WalkmeshSceneNode>(root)->isEnabled()) {
                continue;
            }

            glm::mat4 transform(root->absoluteTransform());
            transform *= glm::translate(root->aabb().center());
            transform *= glm::scale(root->aabb().getSize());

            ShaderUniforms uniforms(_uniformsPrototype);
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.color = glm::vec4(walkmesh ? red : white, 1.0f);

            _shaders.activate(ShaderProgram::SimpleColor, uniforms);
            _meshes.aabb().draw();
        }
    }

    // Render transparent meshes
    for (auto &mesh : _transparentMeshes) {
        mesh->drawSingle(false);
    }

    _context.setBackFaceCullingEnabled(false);

    // Render particles and grass clusters
    for (auto &elem : _elements) {
        int count = elem.first->type() == SceneNodeType::Grass && elem.second.size() > kMaxGrassClusters ? kMaxGrassClusters : -1;
        elem.first->drawElements(elem.second, count);
    }

    // Render lens flares
    for (auto &light : _lights) {
        // Ignore lights that are too far away or outside of camera frustum
        float flareRadius = light->modelNode().light()->flareRadius;
        if (_activeCamera->getDistanceTo2(*light) > flareRadius * flareRadius ||
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

    if (!_lightingRefNode)
        return vector<LightSceneNode *>();

    // Compute distances between each light and the reference node
    vector<pair<LightSceneNode *, float>> distances;

    for (auto &light : _lights) {
        if (!predicate(*light))
            continue;

        // Only account for lights whose distance to the reference node is
        // within range of the light.
        float distance = light->getDistanceTo2(*_lightingRefNode);
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

        if (leftPriority < rightPriority)
            return true;
        if (leftPriority > rightPriority)
            return false;

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

    for (auto &root : _roots) {
        if (root->type() != SceneNodeType::Walkmesh) {
            continue;
        }
        auto walkmesh = static_pointer_cast<WalkmeshSceneNode>(root);
        if (!walkmesh->isEnabled()) {
            continue;
        }
        auto objSpaceOrigin = glm::vec3(walkmesh->absoluteTransformInverse() * glm::vec4(origin, 1.0f));
        float distance = 0.0f;
        auto face = walkmesh->walkmesh().raycast(_walkcheckSurfaces, objSpaceOrigin, down, 2.0f * kElevationTestZ, distance);
        if (face) {
            if (_walkableSurfaces.count(face->material) == 0) {
                // non-walkable
                return false;
            }
            outCollision.user = walkmesh->user();
            outCollision.intersection = origin + distance * down;
            outCollision.normal = walkmesh->absoluteTransform() * glm::vec4(face->normal, 0.0f);
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

    for (auto &root : _roots) {
        if (root->type() != SceneNodeType::Walkmesh) {
            continue;
        }
        if (root->user() == excludeUser) {
            continue;
        }
        auto walkmesh = static_pointer_cast<WalkmeshSceneNode>(root);
        if (!walkmesh->isEnabled()) {
            continue;
        }
        glm::vec3 objSpaceOrigin(walkmesh->absoluteTransformInverse() * glm::vec4(origin, 1.0f));
        glm::vec3 objSpaceDir(walkmesh->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance = 0.0f;
        auto face = walkmesh->walkmesh().raycast(_walkcheckSurfaces, objSpaceOrigin, objSpaceDir, 2.0f * kElevationTestZ, distance);
        if (!face || distance > maxDistance || distance > minDistance) {
            continue;
        }
        outCollision.user = walkmesh->user();
        outCollision.intersection = origin + distance * dir;
        outCollision.normal = walkmesh->absoluteTransform() * glm::vec4(face->normal, 0.0f);
        outCollision.material = face->material;
        minDistance = distance;
    }

    return minDistance != numeric_limits<float>::max();
}

unique_ptr<DummySceneNode> SceneGraph::newDummy(shared_ptr<ModelNode> modelNode) {
    return make_unique<DummySceneNode>(move(modelNode), *this, _context, _meshes, _shaders);
}

unique_ptr<CameraSceneNode> SceneGraph::newCamera(glm::mat4 projection) {
    return make_unique<CameraSceneNode>(
        move(projection),
        *this,
        _context,
        _meshes,
        _shaders);
}

unique_ptr<SoundSceneNode> SceneGraph::newSound() {
    return make_unique<SoundSceneNode>(
        *this,
        _audioPlayer,
        _context,
        _meshes,
        _shaders);
}

unique_ptr<ModelSceneNode> SceneGraph::newModel(shared_ptr<Model> model, ModelUsage usage, IAnimationEventListener *animEventListener) {
    return make_unique<ModelSceneNode>(
        move(model),
        usage,
        *this,
        _context,
        _features,
        _materials,
        _meshes,
        _pbrIbl,
        _shaders,
        _textures,
        animEventListener);
}

unique_ptr<WalkmeshSceneNode> SceneGraph::newWalkmesh(shared_ptr<Walkmesh> walkmesh) {
    return make_unique<WalkmeshSceneNode>(
        move(walkmesh),
        *this,
        _context,
        _meshes,
        _shaders);
}

unique_ptr<GrassSceneNode> SceneGraph::newGrass(glm::vec2 quadSize, shared_ptr<Texture> texture, shared_ptr<Texture> lightmap) {
    return make_unique<GrassSceneNode>(
        move(quadSize),
        move(texture),
        move(lightmap),
        *this,
        _context,
        _meshes,
        _shaders);
}

} // namespace scene

} // namespace reone
