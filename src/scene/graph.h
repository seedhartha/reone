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

#pragma once

#include "../graphics/options.h"
#include "../graphics/scene.h"
#include "../graphics/uniforms.h"

#include "node/camera.h"
#include "node/dummy.h"
#include "node/emitter.h"
#include "node/grass.h"
#include "node/light.h"
#include "node/mesh.h"
#include "user.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class Meshes;
class Shaders;
class Textures;
class Walkmesh;

} // namespace graphics

namespace audio {

class AudioPlayer;

}

namespace scene {

class Collision;
class IAnimationEventListener;
class ModelSceneNode;
class SoundSceneNode;
class WalkmeshSceneNode;

class SceneGraph : public graphics::IScene, boost::noncopyable {
public:
    SceneGraph(
        std::string name,
        graphics::GraphicsOptions options,
        audio::AudioPlayer &audioPlayer,
        graphics::GraphicsContext &graphicsContext,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Textures &textures) :
        _name(std::move(name)),
        _options(std::move(options)),
        _audioPlayer(audioPlayer),
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures) {
    }

    void update(float dt);

    void draw() override;
    void drawShadows() override;

    const std::string &name() const { return _name; }
    const graphics::GraphicsOptions &options() const { return _options; }
    std::shared_ptr<CameraSceneNode> activeCamera() const { return _activeCamera; }

    std::shared_ptr<graphics::Camera> camera() const override {
        return _activeCamera ? _activeCamera->camera() : nullptr;
    }

    void setUpdateRoots(bool update) { _updateRoots = update; }
    void setActiveCamera(std::shared_ptr<CameraSceneNode> camera) { _activeCamera = std::move(camera); }

    // Roots

    void clear();

    void addRoot(std::shared_ptr<ModelSceneNode> node);
    void addRoot(std::shared_ptr<WalkmeshSceneNode> node);
    void addRoot(std::shared_ptr<SoundSceneNode> node);
    void addRoot(std::shared_ptr<GrassSceneNode> node);

    void removeRoot(const std::shared_ptr<ModelSceneNode> &node);
    void removeRoot(const std::shared_ptr<WalkmeshSceneNode> &node);
    void removeRoot(const std::shared_ptr<SoundSceneNode> &node);
    void removeRoot(const std::shared_ptr<GrassSceneNode> &node);

    // END Roots

    // Lighting

    std::vector<LightSceneNode *> computeClosestLights(int count, const std::function<bool(const LightSceneNode &, float)> &pred) const;

    const glm::vec3 &ambientLightColor() const override { return _ambientLightColor; }
    const std::vector<LightSceneNode *> activeLights() const { return _activeLights; }

    void setAmbientLightColor(glm::vec3 color) { _ambientLightColor = std::move(color); }

    // END Lighting

    // Fog

    bool isFogEnabled() const { return _fogEnabled; }

    float fogNear() const override { return _fogNear; }
    float fogFar() const override { return _fogFar; }
    const glm::vec3 &fogColor() const override { return _fogColor; }

    void setFogEnabled(bool enabled) { _fogEnabled = enabled; }
    void setFogNear(float near) { _fogNear = near; }
    void setFogFar(float far) { _fogFar = far; }
    void setFogColor(glm::vec3 color) { _fogColor = std::move(color); }

    // END Fog

    // Shadows

    bool hasShadowLight() const override { return _shadowLight; }
    bool isShadowLightDirectional() const override { return _shadowLight->isDirectional(); }

    glm::vec3 shadowLightPosition() const override { return _shadowLight->absoluteTransform()[3]; }
    float shadowStrength() const override { return _shadowStrength; }
    float shadowRadius() const override { return _shadowLight->radius(); }

    // END Shadows

    // Collision detection and object picking

    bool testElevation(const glm::vec2 &position, Collision &outCollision) const;
    bool testLineOfSight(const glm::vec3 &origin, const glm::vec3 &dest, Collision &outCollision) const;
    bool testWalk(const glm::vec3 &origin, const glm::vec3 &dest, const IUser *excludeUser, Collision &outCollision) const;

    std::shared_ptr<ModelSceneNode> pickModelAt(int x, int y, IUser *except = nullptr) const;

    void setWalkableSurfaces(std::set<uint32_t> surfaces) { _walkableSurfaces = std::move(surfaces); }
    void setWalkcheckSurfaces(std::set<uint32_t> surfaces) { _walkcheckSurfaces = std::move(surfaces); }
    void setLineOfSightSurfaces(std::set<uint32_t> surfaces) { _lineOfSightSurfaces = std::move(surfaces); }

    // END Collision detection and object picking

    // Factory methods

    std::unique_ptr<DummySceneNode> newDummy(std::shared_ptr<graphics::ModelNode> modelNode);
    std::unique_ptr<CameraSceneNode> newCamera();
    std::unique_ptr<SoundSceneNode> newSound();

    std::unique_ptr<ModelSceneNode> newModel(
        std::shared_ptr<graphics::Model> model,
        ModelUsage usage,
        IAnimationEventListener *animEventListener = nullptr);

    std::unique_ptr<WalkmeshSceneNode> newWalkmesh(std::shared_ptr<graphics::Walkmesh> walkmesh);

    std::unique_ptr<GrassSceneNode> newGrass(
        float density,
        float quadSize,
        glm::vec4 probabilities,
        std::set<uint32_t> materials,
        std::shared_ptr<graphics::Texture> texture,
        std::shared_ptr<graphics::ModelNode> aabbNode);

    // END Factory methods

private:
    std::string _name;
    graphics::GraphicsOptions _options;

    bool _updateRoots {true};

    std::shared_ptr<CameraSceneNode> _activeCamera;
    std::vector<LightSceneNode *> _flareLights;

    // Services

    audio::AudioPlayer &_audioPlayer;

    graphics::GraphicsContext &_graphicsContext;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;

    // END Services

    // Roots

    std::set<std::shared_ptr<ModelSceneNode>> _modelRoots;
    std::set<std::shared_ptr<WalkmeshSceneNode>> _walkmeshRoots;
    std::set<std::shared_ptr<SoundSceneNode>> _soundRoots;
    std::set<std::shared_ptr<GrassSceneNode>> _grassRoots;

    // END Roots

    // Leafs

    std::vector<MeshSceneNode *> _opaqueMeshes;
    std::vector<MeshSceneNode *> _transparentMeshes;
    std::vector<MeshSceneNode *> _shadowMeshes;
    std::vector<LightSceneNode *> _lights;
    std::vector<EmitterSceneNode *> _emitters;

    std::vector<std::pair<SceneNode *, std::vector<SceneNode *>>> _leafBuckets; // particles and grass clusters

    // END Leafs

    // Lighting

    glm::vec3 _ambientLightColor {0.5f};

    std::vector<LightSceneNode *> _activeLights;

    // END Lighting

    // Shadows

    bool _shadowActive {false};
    float _shadowStrength {0.0f};

    LightSceneNode *_shadowLight {nullptr};

    // END Shadows

    // Fog

    bool _fogEnabled {false};
    float _fogNear {0.0f};
    float _fogFar {0.0f};
    glm::vec3 _fogColor {0.0f};

    // END Fog

    // Surfaces

    std::set<uint32_t> _walkableSurfaces;
    std::set<uint32_t> _walkcheckSurfaces;
    std::set<uint32_t> _lineOfSightSurfaces;

    // END Surfaces

    void cullRoots();

    void refreshNodeLists();
    void refreshFromSceneNode(const std::shared_ptr<SceneNode> &node);

    void updateLighting();
    void updateShadowLight(float dt);
    void updateFlareLights();
    void updateSounds();

    void prepareTransparentMeshes();
    void prepareLeafs();
};

} // namespace scene

} // namespace reone
