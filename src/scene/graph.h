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
#include "../graphics/shaders.h"

#include "node/camera.h"
#include "node/dummy.h"
#include "node/emitter.h"
#include "node/grass.h"
#include "node/light.h"
#include "node/mesh.h"
#include "user.h"

namespace reone {

namespace graphics {

class Context;
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

class SceneGraph : boost::noncopyable {
public:
    SceneGraph(
        std::string name,
        graphics::GraphicsOptions options,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Textures &textures) :
        _name(std::move(name)),
        _options(std::move(options)),
        _audioPlayer(audioPlayer),
        _context(context),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures) {
    }

    void update(float dt);
    void draw(bool shadowPass = false);

    const std::string &name() const { return _name; }
    const graphics::GraphicsOptions &options() const { return _options; }
    std::shared_ptr<CameraSceneNode> activeCamera() const { return _activeCamera; }
    graphics::ShaderUniforms uniformsPrototype() const { return _uniformsPrototype; }

    void setUpdateRoots(bool update) { _updateRoots = update; }
    void setActiveCamera(std::shared_ptr<CameraSceneNode> camera) { _activeCamera = std::move(camera); }
    void setUniformsPrototype(graphics::ShaderUniforms &&uniforms) { _uniformsPrototype = uniforms; }

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

    // Collision detection

    bool testElevation(const glm::vec2 &position, Collision &outCollision) const;
    bool testObstacle(const glm::vec3 &origin, const glm::vec3 &dest, const IUser *excludeUser, Collision &outCollision) const;

    void setWalkableSurfaces(std::set<uint32_t> surfaces) { _walkableSurfaces = std::move(surfaces); }
    void setWalkcheckSurfaces(std::set<uint32_t> surfaces) { _walkcheckSurfaces = std::move(surfaces); }

    // END Collision detection

    // Lighting and shadows

    /**
     * Get up to count lights, sorted by priority and proximity to the reference node.
     */
    std::vector<LightSceneNode *> getLightsAt(
        int count = graphics::kMaxLights,
        std::function<bool(const LightSceneNode &)> predicate = [](auto &light) { return true; }) const;

    const glm::vec3 &ambientLightColor() const { return _ambientLightColor; }
    const std::vector<LightSceneNode *> closestLights() const { return _closestLights; }
    const LightSceneNode *shadowLight() const { return _shadowLight; }

    void setAmbientLightColor(glm::vec3 color) { _ambientLightColor = std::move(color); }
    void setLightingRefNode(std::shared_ptr<SceneNode> node) { _lightingRefNode = std::move(node); }

    // END Lighting and shadows

    // Fog

    bool isFogEnabled() const { return _fogEnabled; }

    float fogNear() const { return _fogNear; }
    float fogFar() const { return _fogFar; }
    const glm::vec3 &fogColor() const { return _fogColor; }

    void setFogEnabled(bool enabled) { _fogEnabled = enabled; }
    void setFogNear(float near) { _fogNear = near; }
    void setFogFar(float far) { _fogFar = far; }
    void setFogColor(glm::vec3 color) { _fogColor = std::move(color); }

    // END Fog

    // Factory methods

    std::unique_ptr<DummySceneNode> newDummy(std::shared_ptr<graphics::ModelNode> modelNode);
    std::unique_ptr<CameraSceneNode> newCamera(glm::mat4 projection);
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

    std::shared_ptr<CameraSceneNode> _activeCamera;

    uint32_t _textureId {0};
    bool _updateRoots {true};
    graphics::ShaderUniforms _uniformsPrototype;

    std::set<uint32_t> _walkableSurfaces;
    std::set<uint32_t> _walkcheckSurfaces;

    // Services

    audio::AudioPlayer &_audioPlayer;

    graphics::Context &_context;
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

    std::vector<std::pair<SceneNode *, std::vector<SceneNode *>>> _leafs; // particles and grass clusters

    // END Leafs

    // Lighting and shadows

    glm::vec3 _ambientLightColor {0.5f};
    std::shared_ptr<SceneNode> _lightingRefNode; /**< reference node to use when selecting closest light sources */
    std::vector<LightSceneNode *> _closestLights;
    const LightSceneNode *_shadowLight {nullptr};

    // END Lighting and shadows

    // Fog

    bool _fogEnabled {false};
    float _fogNear {0.0f};
    float _fogFar {0.0f};
    glm::vec3 _fogColor {0.0f};

    // END Fog

    void cullRoots();
    void updateLighting();
    void updateSounds();

    void refreshNodeLists();
    void refreshFromSceneNode(const std::shared_ptr<SceneNode> &node);

    void prepareTransparentMeshes();
    void prepareLeafs();
};

} // namespace scene

} // namespace reone
